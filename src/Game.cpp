#include "Game.h"
#include <iostream>

Game::Game()
    : mIsRunning(true),
      mTicksCount(0),
      mSceneManager(SceneManager::GetInstance()),
      mBGM(nullptr),
      mSoundEffect(nullptr)
{
}

bool Game::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
  {
    SDL_Log("SDLの初期化に失敗しました: %s", SDL_GetError());
    return false;
  }

  if (TTF_Init() == -1)
  {
    SDL_Log("SDL_ttfの初期化に失敗しました: %s", TTF_GetError());
    return false;
  }

  if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0)
  {
    SDL_Log("SDL_mixerの初期化に失敗しました: %s", Mix_GetError());
    return false;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    SDL_Log("SDL_mixerのオーディオを開くのに失敗しました: %s", Mix_GetError());
    return false;
  }

  LoadAssets();

  // シーンの作成と変更をより安全に行う
  auto startScene = std::make_unique<StartScene>();
  mSceneManager.ChangeScene(std::move(startScene)); // メンバ変数経由でアクセス
  return true;
}

void Game::LoadAssets()
{
  // BGMのロード
  mBGM = Mix_LoadMUS("assets/sounds/bgm.mp3");
  if (!mBGM)
  {
    std::cerr << "BGMのロードに失敗しました: " << Mix_GetError() << std::endl;
  }

  // 効果音のロード
  mSoundEffect = Mix_LoadWAV("assets/sounds/effect.wav");
  if (!mSoundEffect)
  {
    std::cerr << "効果音のロードに失敗しました: " << Mix_GetError() << std::endl;
  }

  // BGMの再生
  if (mBGM)
  {
    Mix_PlayMusic(mBGM, -1); // ループ再生
  }
}

void Game::Shutdown()
{
  // サウンドの解放
  if (mSoundEffect)
  {
    Mix_FreeChunk(mSoundEffect);
    mSoundEffect = nullptr;
  }

  if (mBGM)
  {
    Mix_FreeMusic(mBGM);
    mBGM = nullptr;
  }

  Mix_CloseAudio();
  Mix_Quit();

  mSceneManager.Shutdown();
  TTF_Quit();
  SDL_Quit();
}

void Game::RunLoop()
{
  while (mIsRunning)
  {
    ProcessInput();
    UpdateGame();
    GenerateOutput();
  }
}

void Game::ProcessInput()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
      mIsRunning = false;
      break;
    default:
      mSceneManager.HandleEvent(event);
      break;
    }
  }

  // キーボード状態の取得
  const Uint8 *keyBoardState = SDL_GetKeyboardState(NULL);
  if (keyBoardState[SDL_SCANCODE_O])
  {
    mIsRunning = false;
  }

  mSceneManager.HandleInput(keyBoardState);
}

void Game::UpdateGame()
{
  float deltaTime = 0.0f;
  Uint32 newTicks = SDL_GetTicks();
  deltaTime = static_cast<float>(newTicks - mTicksCount) / 1000.0f;
  mTicksCount = newTicks;

  mSceneManager.Update(deltaTime);
}

void Game::GenerateOutput()
{
  mSceneManager.Draw();
}
