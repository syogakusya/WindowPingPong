#include "GamePlayScene.h"
#include <iostream>

const int MASTER_WINDOW_HEIGHT = 160;
const int WINDOW_SIZE = 200;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 6;
const float MAX_BALL_SPEED = 500.0f;

GamePlayScene::GamePlayScene()
    : mPrevSpaceKeyState(false),
      mScore(0),
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
  mBall->SetVelocity(Vector2(-120.0f, 135.0f));
  mBalls.push_back(std::move(mBall));
  mPaddle = std::unique_ptr<Paddle>(
      new Paddle(
          Vector2(mScreen->x / 4, mScreen->y / 2),
          Vector2(WINDOW_SIZE, WINDOW_SIZE),
          PADDLE_WIDTH, PADDLE_HEIGHT, mMasterWindow->GetOffSetY()));

  mPixelifySansRenderer = std::unique_ptr<TextRenderer>(
      new TextRenderer("PixelifySans-VariableFont_wght.ttf", 24));

  mCurrentState = GameState::Playing;

  SDL_Color normalColor = {100, 100, 100, 255};
  SDL_Color hoverColor = {150, 150, 150, 255};
  mRestartButton = std::make_unique<Button>(
      "Restart",
      Vector2(mScreen->x / 2 - 50, 10),
      Vector2(100, 40),
      normalColor,
      hoverColor);

  mRestartButton->SetOnClick([this]()
                             {
      // リスタート処理
      mScore = 0;
      // ボールの位置とスピードをリセット
    for (auto &ball : mBalls)
    {
      ball->SetVelocity(Vector2(-120.0f, 135.0f));
      ball->SetWorldPos(Vector2(mScreen->x / 2, mScreen->y / 2));
      } });
}

void GamePlayScene::HandleInput(const Uint8 *keyBoardState)
{
  // キーボード
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

  // マウス
  int mouseX, mouseY;
  Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
  mLastMousePos = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));

  bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
  if (isMousePressed && !mMousePressed)
  {
    mRestartButton->HandleClick(mLastMousePos);
  }
  mMousePressed = isMousePressed;
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

  mRestartButton->Update(mLastMousePos);
}

void GamePlayScene::Draw()
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
  mRestartButton->Draw(mMasterWindow->GetRenderer(), mPixelifySansRenderer.get());

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
  SDL_Rect paddleRect = mPaddle->GetPaddleRect();
  if (ball->CheckBallCollision(&paddleRect) && !isBallCollision)
  {
    ball->ReverseVelocityX();
    // mPaddle->StartShake(0.1f, 5.0f);
    mScore += 100;

    Vector2 currentVel = ball->GetVelocity();
    float currentSpeed = std::sqrt(currentVel.x * currentVel.x + currentVel.y * currentVel.y);
    if (currentSpeed < MAX_BALL_SPEED)
    {
      ball->SetVelocity(currentVel * 1.02f);
    }

    isBallCollision = true;
  }
  else if (!ball->CheckBallCollision(&paddleRect))
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
