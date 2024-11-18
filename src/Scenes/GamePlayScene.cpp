#include "GamePlayScene.h"
#include <iostream>

using namespace std;

const int MASTER_WINDOW_HEIGHT = 160;
const int WINDOW_SIZE = 200;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 6;
const float MAX_BALL_SPEED = 500.0f;
const float OBSTACLE_SPAWN_INTERVAL = 15.0f; // 障害物生成間隔

GamePlayScene::GamePlayScene()
    : mPrevSpaceKeyState(false),
      mScore(0),
      isBallCollision(false),
      mCurrentState(GameState::Start),
      mObstacleSpawnTimer(OBSTACLE_SPAWN_INTERVAL),
      prevBallReverseX(false),
      prevBallReverseY(false),
      isPaddleObstacleCollision(false)
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

  std::cout << mScreen->x << " " << mScreen->y << std::endl;

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

  // std::cout << mMasterWindow->GetOffSetY() << std::endl;
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

    // マウスのワールド座標
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetGlobalMouseState(&mouseX, &mouseY);
    mWorldMousePos = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));

    bool isMousePressed = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
    if (isMousePressed && !mMousePressed)
    {
      mRestartButton->HandleClick(mWorldMousePos);
    }
    mMousePressed = isMousePressed;
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
      CheckBallCollisions(ball);
    }
    mPaddle->Update(deltaTime);

    UpdateObstacles(deltaTime);
  }

  mRestartButton->Update(mLocalMousePos);

  // パドルがマウスに追従するモード時の衝突判定
  if (mPaddle->IsFollowingMouse())
  {
    SDL_Rect paddleRect = mPaddle->GetWindowRect();
    bool isCollision = false;
    for (const auto &obstacle : mObstacles)
    {
      SDL_Rect obstacleRect = obstacle->GetWindowRect();
      SDL_Point mousePos = {static_cast<int>(mLocalMousePos.x), static_cast<int>(mLocalMousePos.y)};
      if (obstacle->GetType() == ObstacleWindow::Type::Popup &&
          (SDL_HasIntersection(&paddleRect, &obstacleRect) ||
           SDL_PointInRect(&mousePos, &obstacleRect)))
      {
        isCollision = true;
        if (!isPaddleObstacleCollision)
        {
          isPaddleObstacleCollision = true;
          mLastWorldMousePos = mWorldMousePos;
        }

        // 現在のマウス位置と前回の安全な位置との間で補間を行う
        Vector2 direction = mWorldMousePos - mLastWorldMousePos;
        direction.Normalize();

        // 障害物の境界からの距離を確保
        const float SAFE_DISTANCE = 10.0f; // 安全マージン

        Vector2 safePos = mLastWorldMousePos;
        SDL_Rect testRect = paddleRect;

        // 最大試行回数を設定
        const int MAX_ATTEMPTS = 10;
        int attempts = 0;

        // 障害物との衝突がなくなるまで位置を調整（ただし最大試行回数まで）
        while (SDL_HasIntersection(&testRect, &obstacleRect) && attempts < MAX_ATTEMPTS)
        {
          safePos = safePos - direction * SAFE_DISTANCE;
          testRect.x = static_cast<int>(safePos.x - mPaddle->GetWindowSize().x / 2);
          testRect.y = static_cast<int>(safePos.y - mPaddle->GetWindowSize().y / 2);
          attempts++;
        }

        // 最大試行回数に達した場合は、最後の安全な位置に強制的に戻す
        if (attempts >= MAX_ATTEMPTS)
        {
          safePos = mLastWorldMousePos;
        }

        mPaddle->SetWindowPos(safePos - mPaddle->GetWindowSize() / 2.0f);
        mPaddle->SetWorldPos(safePos);
        break;
      }
    }
    if (!isCollision)
    {
      isPaddleObstacleCollision = false;
    }
  }
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

  // すべてのRenderPresentを呼び出す
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

void GamePlayScene::CheckBallCollisions(std::unique_ptr<Ball> &ball)
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

  // 障害物との衝突判定
  for (const auto &obstacle : mObstacles)
  {
    SDL_Rect ballRect = ball->GetBallRect();
    if (obstacle->CheckWindowCollision(&ballRect))
    {
      Vector2 ballPos = ball->GetWorldPos();
      Vector2 obstaclePos = obstacle->GetWindowPos();
      Vector2 obstacleSize = obstacle->GetWindowSize();

      // 上下の衝突判定
      if (ballPos.y < obstaclePos.y + ball->GetBallSize() / 2.0f ||
          ballPos.y > obstaclePos.y + obstacleSize.y - ball->GetBallSize() / 2.0f)
      {
        if (!prevBallReverseY)
        {
          ball->ReverseVelocityY();
          prevBallReverseY = true;
        }
      }
      else
      {
        prevBallReverseY = false;
      }

      // 左右の衝突判定
      if (ballPos.x < obstaclePos.x + ball->GetBallSize() / 2.0f ||
          ballPos.x > obstaclePos.x + obstacleSize.x - ball->GetBallSize() / 2.0f)
      {
        if (!prevBallReverseX)
        {
          ball->ReverseVelocityX();
          prevBallReverseX = true;
        }
      }
      else
      {
        prevBallReverseX = false;
      }
    }
    else
    {
      prevBallReverseX = false;
      prevBallReverseY = false;
    }
  }
}

void GamePlayScene::AddBall(Vector2 pos, Vector2 velocity)
{
  mBall = std::unique_ptr<Ball>(
      new Ball(pos, Vector2(WINDOW_SIZE, WINDOW_SIZE), BALL_SIZE, mMasterWindow->GetOffSetY()));
  mBall->SetVelocity(velocity);
  mBalls.push_back(std::move(mBall));
}

void GamePlayScene::SpawnObstacle()
{
  // ランダムな位置を生成（ただしパドルとボールの位置を避ける）
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> xDist(100, mScreen->x - 100);
  std::uniform_real_distribution<float> yDist(mMasterWindow->GetOffSetY() + 100, mScreen->y - 100);
  std::uniform_int_distribution<int> typeDist(0, 1);

  Vector2 pos;
  bool validPosition = false;
  while (!validPosition)
  {
    pos = Vector2(xDist(gen), yDist(gen));
    validPosition = !CheckObstacleCollision(pos);
  }

  ObstacleWindow::Type type = typeDist(gen) == 0 ? ObstacleWindow::Type::Transparent : ObstacleWindow::Type::Popup;

  float duration = type == ObstacleWindow::Type::Transparent ? 10.0f : 0.0f;
  int closeCount = type == ObstacleWindow::Type::Popup ? std::uniform_int_distribution<int>(1, 3)(gen) : 1;

  mObstacles.push_back(std::make_unique<ObstacleWindow>(
      type == ObstacleWindow::Type::Transparent ? "Obstacle" : "Advertisement",
      pos,
      Vector2(200, 200),
      type,
      duration,
      closeCount));
}

void GamePlayScene::UpdateObstacles(float deltaTime)
{
  mObstacleSpawnTimer -= deltaTime;
  if (mObstacleSpawnTimer <= 0.0f)
  {
    SpawnObstacle();
    mObstacleSpawnTimer = OBSTACLE_SPAWN_INTERVAL;
  }

  // 妨害ウィンドウの更新と期限切れの削除
  auto it = mObstacles.begin();
  while (it != mObstacles.end())
  {
    if (!(*it)->IsActive())
    {
      it = mObstacles.erase(it);
    }
    else
    {
      (*it)->Update(deltaTime);
      ++it;
    }
  }
}

// 障害物が出現する場所がパドルやボールに重なっているか確かめるためのメソッド
bool GamePlayScene::CheckObstacleCollision(const Vector2 &pos) const
{
  SDL_Rect newRect = {
      static_cast<int>(pos.x),
      static_cast<int>(pos.y),
      200, 200};

  // パドルとの衝突チェック
  SDL_Rect paddleRect = mPaddle->GetPaddleRect();
  if (SDL_HasIntersection(&newRect, &paddleRect))
    return true;

  // ボールとの衝突チェック
  for (const auto &ball : mBalls)
  {
    SDL_Rect ballRect = ball->GetBallRect();
    if (SDL_HasIntersection(&newRect, &ballRect))
      return true;
  }

  return false;
}

// SDL_Eventのイベントハンドラー
void GamePlayScene::HandleEvent(const SDL_Event &event)
{

  switch (event.type)
  {
  case SDL_MOUSEMOTION:
    mLocalMousePos = Vector2(static_cast<float>(event.motion.x),
                             static_cast<float>(event.motion.y));
    break;

  case SDL_MOUSEBUTTONDOWN:
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      mMousePressed = true;
      mRestartButton->HandleClick(mLocalMousePos);
    }
    break;

  case SDL_MOUSEBUTTONUP:
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      mMousePressed = false;
    }
    break;

  case SDL_WINDOWEVENT:
    if (event.window.event == SDL_WINDOWEVENT_CLOSE)
    {
      for (auto &obstacle : mObstacles)
      {
        if (obstacle->GetType() == ObstacleWindow::Type::Popup &&
            SDL_GetWindowID(obstacle->GetWindow()) == event.window.windowID)
        {
          obstacle->HandleClick();
          break;
        }
      }
    }
    break;
  }
}
