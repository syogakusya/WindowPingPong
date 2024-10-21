#include "Game.h"
#include "MasterWindow.h"

const int MASTER_WINDOW_HEIGHT = 160;
const int BALL_SIZE = 200;
const int PADDLE_WIDTH = 15;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;

Game::Game()
    : mIsRunning(true),
      mTicksCount(0),
      mPrevSpaceKeyState(false),
      mCurrentState(GameState::Start),
      mScore(0)
{
}

bool Game::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    SDL_Log("SDLの初期化に失敗しました: %s", SDL_GetError());
    return false;
  }

  SDL_DisplayMode displayMode;
  if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
  {
    mScreen->x = displayMode.w;
    mScreen->y = displayMode.h;
  }

  mMasterWindow =
      std::unique_ptr<MasterWindow>(
          new MasterWindow(
              "WindowsPingPong", 0, 0,
              mScreen->x,
              MASTER_WINDOW_HEIGHT, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS));
  mBall = std::unique_ptr<Ball>(
      new Ball(mScreen->x / 2, mScreen->y / 2, BALL_SIZE));
  mPaddle = std::unique_ptr<Paddle>(
      new Paddle(PADDLE_WIDTH, mScreen->y / 2, PADDLE_WIDTH, PADDLE_HEIGHT));

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
}

void Game::CheckCollisions()
{
  // ボールとパドルの衝突判定
  Vector2 ballPos = mBall->worldPos;
  Vector2 paddlePos = mPaddle->worldPos;
  // Vector2 ballVel = mBall->velocity; // 未使用なので削除または使用

  if (ballPos.y - BALL_SIZE / 2.0f < paddlePos.y + PADDLE_HEIGHT / 2.0f &&
      ballPos.y + BALL_SIZE / 2.0f > paddlePos.y - PADDLE_HEIGHT / 2.0f &&
      paddlePos.x - PADDLE_WIDTH / 2.0f < ballPos.x + BALL_SIZE / 2.0f &&
      paddlePos.x + PADDLE_WIDTH / 2.0f > ballPos.x - BALL_SIZE / 2.0f)
  {
    mBall->ReverseVelocityX();
    mScore++;
  }

  // ボールと画面端の衝突判定
  if (ballPos.x <= 0 || ballPos.x >= SCREEN_WIDTH)
  {
    mBall->ReverseVelocityX();
  }
  if (ballPos.y <= MASTER_WINDOW_HEIGHT || ballPos.y >= SCREEN_HEIGHT)
  {
    mBall->ReverseVelocityY();
  }
}

void Game::Shutdown()
{
  SDL_Quit();
}
