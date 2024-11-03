#include "GamePlayScene.h"

const int MASTER_WINDOW_HEIGHT = 160;
const int WINDOW_SIZE = 200;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 6;

GamePlayScene::GamePlayScene()
    : mScore(0),
      mPrevSpaceKeyState(false),
      isBallCollision(false),
      mCurrentState(GameState::Start)
{
}

void GamePlayScene::Initialize()
{
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
    exit(1);
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
  mBalls.push_back(std::move(mBall));
  mPaddle = std::unique_ptr<Paddle>(
      new Paddle(
          Vector2(mScreen->x / 4, mScreen->y / 2),
          Vector2(WINDOW_SIZE, WINDOW_SIZE),
          PADDLE_WIDTH, PADDLE_HEIGHT, mMasterWindow->GetOffSetY()));

  mPixelifySansRenderer = std::unique_ptr<TextRenderer>(
      new TextRenderer("PixelifySans-VariableFont_wght.ttf", 24));

  mCurrentState = GameState::Playing;
}

void GamePlayScene::HandleInput(const Uint8 *keyBoardState)
{
  float paddleDir = 0.0f;
  if (mCurrentState == GameState::Playing)
  {
    if (keyBoardState[SDL_SCANCODE_UP] || keyBoardState[SDL_SCANCODE_W])
    {
      paddleDir = -1.0f;
    }
    if (keyBoardState[SDL_SCANCODE_DOWN] || keyBoardState[SDL_SCANCODE_S])
    {
      paddleDir = 1.0f;
    }
    mPaddle->SetDirection(paddleDir);

    static bool prevCKeyState = false;
    bool currentCKeyState = keyBoardState[SDL_SCANCODE_C];
    if (currentCKeyState && !prevCKeyState)
    {
      mPaddle->ToggleMouseFollow();
    }
    prevCKeyState = currentCKeyState;
  }

  bool spaceDown = keyBoardState[SDL_SCANCODE_SPACE];
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

void GamePlayScene::Update(float deltaTime)
{
  if (mCurrentState == GameState::Playing)
  {
    for (auto &ball : mBalls)
    {
      ball->Update(deltaTime);
      CheckCollisions(ball);
    }
    mPaddle->Update(deltaTime);
  }
}

void GamePlayScene::Render()
{
  mMasterWindow->Draw(mMasterWindow->GetRenderer());
  for (auto &ball : mBalls)
  {
    ball->Draw(ball->GetRenderer());
  }
  mPaddle->Draw(mPaddle->GetRenderer());
  for (auto &ball : mBalls)
  {
    mPaddle->DrawBall(mPaddle->GetRenderer(), ball.get());
  }

  SDL_Color textColor = {255, 255, 255, 255};
  mPixelifySansRenderer->RenderText(
      "Score: " + std::to_string(mScore),
      10,
      10,
      textColor,
      mMasterWindow->GetRenderer());

  // RenderPresent
  for (auto &ball : mBalls)
  {
    ball->RenderPresent(ball->GetRenderer());
  }
  mPaddle->RenderPresent(mPaddle->GetRenderer());
  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());
}

void GamePlayScene::Shutdown()
{
  mBalls.clear();
  mBall.reset();
  mPaddle.reset();
  mMasterWindow.reset();
  mPixelifySansRenderer.reset();
  mScreen.reset();
}

void GamePlayScene::CheckCollisions(std::unique_ptr<Ball> &ball)
{
  // ボールとパドルの衝突判定
  Vector2 ballPos = ball->GetWorldPos();
  Vector2 paddlePos = mPaddle->GetWorldPos();

  if (ballPos.y - BALL_SIZE / 2.0f < paddlePos.y + PADDLE_HEIGHT / 2.0f &&
      ballPos.y + BALL_SIZE / 2.0f > paddlePos.y - PADDLE_HEIGHT / 2.0f &&
      paddlePos.x - PADDLE_WIDTH / 2.0f < ballPos.x + BALL_SIZE / 2.0f &&
      paddlePos.x + PADDLE_WIDTH / 2.0f > ballPos.x - BALL_SIZE / 2.0f)
  {
    if (isBallCollision == false)
    {
      ball->ReverseVelocityX();
      mScore++;
    }
    isBallCollision = true;
  }
  else
  {
    isBallCollision = false;
  }
}

void GamePlayScene::AddBall(Vector2 pos, Vector2 velocity)
{
  mBall = std::unique_ptr<Ball>(
      new Ball(pos, Vector2(WINDOW_SIZE, WINDOW_SIZE), BALL_SIZE, mMasterWindow->GetOffSetY()));
  mBall->SetVelocity(velocity);
  mBalls.push_back(std::move(mBall));
}
