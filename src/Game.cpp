#include "Game.h"
#include "MasterWindow.h"

const int MASTER_WINDOW_HEIGHT = 160;
const int WINDOW_SIZE = 200;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 8;

Game::Game()
    : mIsRunning(true),
      mTicksCount(0),
      mPrevSpaceKeyState(false),
      mCurrentState(GameState::Start),
      mScore(0),
      isBallCollision(false)
{
}

bool Game::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    SDL_Log("SDLの初期化に失敗しました: %s", SDL_GetError());
    return false;
  }

  mScreen = std::unique_ptr<Vector2>(new Vector2());
  SDL_DisplayMode displayMode;
  if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
  {
    mScreen->x = displayMode.w;
    mScreen->y = displayMode.h;

    GameObject::mScreenSize = *mScreen;

    printf("mScreen->x: %f, mScreen->y: %f\n", mScreen->x, mScreen->y);
  }
  else
  {
    SDL_Log("ディスプレイモードの取得に失敗しました: %s", SDL_GetError());
    return false;
  }

  mMasterWindow =
      std::unique_ptr<MasterWindow>(
          new MasterWindow(
              "WindowsPingPong",
              Vector2(mScreen->x / 2, 0),
              Vector2(mScreen->x, MASTER_WINDOW_HEIGHT),
              UI_MARGIN,
              SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS));
  mBall = std::unique_ptr<Ball>(
      new Ball(
          Vector2(mScreen->x / 2, mScreen->y / 2),
          Vector2(WINDOW_SIZE, WINDOW_SIZE),
          BALL_SIZE,
          mMasterWindow->GetOffSetY()));
  mPaddle = std::unique_ptr<Paddle>(
      new Paddle(
          Vector2(mScreen->x / 4, mScreen->y / 2),
          Vector2(WINDOW_SIZE, WINDOW_SIZE),
          PADDLE_WIDTH, PADDLE_HEIGHT, mMasterWindow->GetOffSetY()));

  mCurrentState = GameState::Playing;
  mIsRunning = true;
  mTicksCount = 0;
  mScore = 0;

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
    }
  }

  mKeyboardState = SDL_GetKeyboardState(NULL);

  if (mKeyboardState[SDL_SCANCODE_ESCAPE])
  {
    mIsRunning = false;
  }

  if (mCurrentState == GameState::Playing)
  {
    float paddleDir = 0.0f;
    if (mKeyboardState[SDL_SCANCODE_UP] || mKeyboardState[SDL_SCANCODE_W])
    {
      paddleDir -= 1.0f;
    }
    if (mKeyboardState[SDL_SCANCODE_DOWN] || mKeyboardState[SDL_SCANCODE_S])
    {
      paddleDir += 1.0f;
    }
    mPaddle->SetDirection(paddleDir);
  }

  bool spaceDown = mKeyboardState[SDL_SCANCODE_SPACE];
  if (spaceDown && !mPrevSpaceKeyState)
  {
    if (mCurrentState == GameState::Playing)
    {
      mCurrentState = GameState::Pause;
    }
    else if (mCurrentState == GameState::Pause)
    {
      mCurrentState = GameState::Playing;
    }
  }
  mPrevSpaceKeyState = spaceDown;
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

  if (mCurrentState == GameState::Playing)
  {
    mBall->Update(deltaTime);
    mPaddle->Update(deltaTime);
    CheckCollisions();
  }
}

void Game::GenerateOutput()
{
  mMasterWindow->Draw(mMasterWindow->GetRenderer());
  mBall->Draw(mBall->GetRenderer());
  mPaddle->Draw(mPaddle->GetRenderer());
  mPaddle->DrawBall(mPaddle->GetRenderer(), mBall.get());

  mBall->RenderPresent(mBall->GetRenderer());
  mPaddle->RenderPresent(mPaddle->GetRenderer());
  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());
}

void Game::CheckCollisions()
{
  // ボールとパドルの衝突判定
  Vector2 ballPos = mBall->GetWorldPos();
  Vector2 paddlePos = mPaddle->GetWorldPos();

  if (ballPos.y - BALL_SIZE / 2.0f < paddlePos.y + PADDLE_HEIGHT / 2.0f &&
      ballPos.y + BALL_SIZE / 2.0f > paddlePos.y - PADDLE_HEIGHT / 2.0f &&
      paddlePos.x - PADDLE_WIDTH / 2.0f < ballPos.x + BALL_SIZE / 2.0f &&
      paddlePos.x + PADDLE_WIDTH / 2.0f > ballPos.x - BALL_SIZE / 2.0f)
  {
    if (isBallCollision == false)
    {
      mBall->ReverseVelocityX();
    }
    isBallCollision = true;
  }
  else
  {
    isBallCollision = false;
  }
}

void Game::Shutdown()
{
  SDL_Quit();
}
