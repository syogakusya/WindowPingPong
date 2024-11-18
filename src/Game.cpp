#include "Game.h"

Game::Game()
    : mIsRunning(true),
      mTicksCount(0),
      mSceneManager(SceneManager::GetInstance()) // シングルトンインスタンスの参照を保持
{
}

bool Game::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    SDL_Log("SDLの初期化に失敗しました: %s", SDL_GetError());
    return false;
  }

  if (TTF_Init() == -1)
  {
    SDL_Log("SDL_ttfの初期化に失敗しました: %s", TTF_GetError());
    return false;
  }

  // シーンの作成と変更をより安全に行う
  auto startScene = std::make_unique<StartScene>();
  mSceneManager.ChangeScene(std::move(startScene)); // メンバ変数経由でアクセス
  return true;
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
  mKeyboardState = SDL_GetKeyboardState(NULL);
  if (mKeyboardState[SDL_SCANCODE_ESCAPE])
  {
    mIsRunning = false;
  }

  mSceneManager.HandleInput(mKeyboardState);
}

void Game::UpdateGame()
{
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
    ;

  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
  mTicksCount = SDL_GetTicks();

  if (deltaTime > 0.05f)
  {
    deltaTime = 0.05f;
  }

  mSceneManager.Update(deltaTime);
}

void Game::GenerateOutput()
{
  mSceneManager.Draw();
}

void Game::Shutdown()
{
  mSceneManager.Shutdown();
  TTF_Quit();
  SDL_Quit();
}
