#include "GamePlayScene.h"

using namespace std;

const int MASTER_WINDOW_HEIGHT = 160;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 6;
const float MAX_BALL_SPEED = 500.0f;
const float OBSTACLE_SPAWN_INTERVAL = 5.0f; // 障害物生成間隔

extern Game *gGameInstance;

GamePlayScene::GamePlayScene()
    : mScore(0),
      mObstacleSpawnTimer(OBSTACLE_SPAWN_INTERVAL),
      isPaddleObstacleCollision(false),
      mHighScoreManager("highscore.txt"),
      mIsGameOver(false)
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

  mWindowSize = mScreen->y / 5;

  mMasterWindow =
      std::unique_ptr<MasterWindow>(
          new MasterWindow(
              "WindowsPingPong",
              Vector2(mScreen->x / 2, MASTER_WINDOW_HEIGHT / 2),
              Vector2(mScreen->x, MASTER_WINDOW_HEIGHT),
              UI_MARGIN,
              SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS));
  auto initialBall = std::unique_ptr<Ball>(
      new Ball(
          Vector2(mScreen->x / 2, mScreen->y / 2),
          Vector2(mWindowSize, mWindowSize),
          BALL_SIZE,
          mMasterWindow->GetOffSetY()));
  initialBall->SetVelocity(Vector2(-120.0f, 135.0f));

  if (initialBall)
  {
    mBalls.push_back(std::move(initialBall));
  }
  else
  {
    throw std::runtime_error("Failed to create initial ball");
  }

  mPaddle = std::unique_ptr<Paddle>(
      new Paddle(
          Vector2(mScreen->x / 4, mScreen->y / 2),
          Vector2(mWindowSize, mWindowSize),
          PADDLE_WIDTH, PADDLE_HEIGHT, mMasterWindow->GetOffSetY()));

  mPixelifySansRenderer = std::unique_ptr<TextRenderer>(
      new TextRenderer("PixelifySans-VariableFont_wght.ttf", 24));

  mCurrentState = GameState::Playing;

  SDL_Color normalColor = {100, 100, 100, 255};
  SDL_Color hoverColor = {150, 150, 150, 255};
  mRestartButton = std::make_unique<Button>(
      "Restart",
      Vector2(mScreen->x / 2 - 500, 80),
      Vector2(200, 40),
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

  normalColor = {100, 100, 100, 255};
  hoverColor = {150, 150, 150, 255};
  mReturnToStartButton = std::make_unique<Button>(
      "Return to Title",
      Vector2(mScreen->x / 2 + 280, 80),
      Vector2(200, 40),
      normalColor,
      hoverColor);

  mReturnToStartButton->SetOnClick([]()
                                   {
      // スタートシーンに戻る処理
      auto startScene = std::make_unique<StartScene>();
      SceneManager::GetInstance().ChangeScene(std::move(startScene)); });

  // std::cout << mMasterWindow->GetOffSetY() << std::endl;
}

void GamePlayScene::HandleInput(const Uint8 *keyBoardState)
{
  // マウスのワールド座標
  int mouseX, mouseY;
  Uint32 mouseState = SDL_GetGlobalMouseState(&mouseX, &mouseY);
  mWorldMousePos = Vector2(static_cast<float>(mouseX), static_cast<float>(mouseY));

  if (mCurrentState == GameState::Playing)
  {
    // キーボード
    float paddleDir = 0.0f;
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
  else if (mCurrentState == GameState::GameOver)
  {
    // ゲームオーバー時の入力処理
    if (keyBoardState[SDL_SCANCODE_SPACE])
    {
      // リスタート処理
      mRestartButton->HandleClick(Vector2(mScreen->x / 2 - 50 + 0, mScreen->y / 2 + 100));
    }

    // ESCキーでスタートシーンに戻る
    if (keyBoardState[SDL_SCANCODE_ESCAPE])
    {
      mReturnToStartButton->HandleClick(Vector2(mScreen->x / 2 + 20 + 0, mScreen->y / 2 + 100));
    }
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
    // ボールの更新と画面外判定
    for (auto it = mBalls.begin(); it != mBalls.end();)
    {
      (*it)->Update(deltaTime);

      // 左右の画面外判定
      Vector2 ballPos = (*it)->GetWorldPos();
      if (ballPos.x < 0 || ballPos.x > mScreen->x)
      {
        it = mBalls.erase(it);
      }
      else
      {
        ++it;
      }
    }
    // ボールがなくなったらGameOver
    if (mBalls.empty())
    {
      mCurrentState = GameState::GameOver;
      std::cout << "GameOver" << std::endl;
      mHighScoreManager.UpdateHighScore(mScore);
      mIsGameOver = true;
    }

    mPaddle->Update(deltaTime);
    UpdateObstacles(deltaTime);

    

    for (auto &ball : mBalls)
    {
      if (ball)
      {
        ball->Update(deltaTime);
        CheckBallCollisions(ball.get());
      }
    }
  }

  mMasterWindow->Update(deltaTime);
  if (mCurrentState == GameState::GameOver)
  {
    mRestartButton->Update(mLocalMousePos);
    mReturnToStartButton->Update(mLocalMousePos);
  }
}

void GamePlayScene::Draw()
{
  mMasterWindow->Draw(mMasterWindow->GetRenderer());

  for (auto &ball : mBalls)
  {
    if (ball)
      ball->Draw(ball->GetRenderer());
  }

  mPaddle->Draw(mPaddle->GetRenderer());

  SDL_Rect paddleRect = mPaddle->GetPaddleRect();
  for (auto &ball : mBalls)
  {
    if (ball)
    {
      SDL_Rect ballRect = ball->GetBallRect();
      ball->DrawRect(ball->GetRenderer(), paddleRect);
      for (auto &ball2 : mBalls)
      {
        if (ball2)
        {
          SDL_Rect ballRect2 = ball2->GetBallRect();
          ball->DrawRect(ball->GetRenderer(), ballRect2);
        }
      }
      mPaddle->DrawRect(mPaddle->GetRenderer(), ballRect);
    }
  }

  SDL_Color textColor = {255, 255, 255, 255};
  mPixelifySansRenderer->RenderText(
      "Score: " + std::to_string(mScore),
      10,
      10,
      textColor,
      mMasterWindow->GetRenderer());
  mPixelifySansRenderer->RenderText(
      "High Score: " + std::to_string(mHighScoreManager.GetHighScore()),
      10,
      40,
      textColor,
      mMasterWindow->GetRenderer());

  if (mCurrentState == GameState::GameOver)
  {
    SDL_Color gameOverColor = {255, 255, 255, 255};
    mPixelifySansRenderer->SetFontSize(96);
    mPixelifySansRenderer->RenderText(
        "GAME OVER",
        mScreen->x / 2 - 255,
        10,
        gameOverColor,
        mMasterWindow->GetRenderer());

    mPixelifySansRenderer->SetFontSize(24);
    mPixelifySansRenderer->RenderText(
        "Space: Restart",
        mScreen->x / 2 - 490,
        40,
        textColor,
        mMasterWindow->GetRenderer());

    mPixelifySansRenderer->RenderText(
        "R: Return to Start",
        mScreen->x / 2 + 270,
        40,
        textColor,
        mMasterWindow->GetRenderer());

    mRestartButton->Draw(mMasterWindow->GetRenderer(), mPixelifySansRenderer.get());
    mReturnToStartButton->Draw(mMasterWindow->GetRenderer(), mPixelifySansRenderer.get());
  }

  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());
  for (auto &ball : mBalls)
  {
    if (ball)
    {
      ball->RenderPresent(ball->GetRenderer());
    }
  }
  mPaddle->RenderPresent(mPaddle->GetRenderer());

  for (auto &obstacle : mObstacles)
  {
    obstacle->Draw(mMasterWindow->GetRenderer());
    obstacle->RenderPresent(obstacle->GetRenderer());
  }
}

void GamePlayScene::Shutdown()
{
  mBalls.clear();
  mPaddle.reset();
  mMasterWindow.reset();
  mPixelifySansRenderer.reset();
  mScreen.reset();
}

void GamePlayScene::CheckBallCollisions(Ball *ball)
{
  if (!ball)
  {
    std::cerr << "Error: CheckBallCollisions called with null ball." << std::endl;
    return;
  }

  SDL_Rect paddleRect = mPaddle->GetPaddleRect();
  if (ball->CheckBallCollision(&paddleRect) && !ball->GetIsCollisionPaddle())
  {
    ball->SetIsCollisionPaddle(true);
    ball->ReverseVelocityX();
    mScore += 100;

    if (gGameInstance && gGameInstance->GetSoundEffect())
    {
      Mix_PlayChannel(-1, gGameInstance->GetSoundEffect(), 0);
    }

    Vector2 currentVel = ball->GetVelocity();
    float currentSpeed = std::sqrt(currentVel.x * currentVel.x + currentVel.y * currentVel.y);
    if (currentSpeed < MAX_BALL_SPEED)
    {
      ball->SetVelocity(currentVel * 1.02f);
    }
    if (mScore % 1000 == 0 && mScore != 0)
    {
      AddBall(Vector2(mScreen->x / 2, mScreen->y / 2), Vector2(-120.0f, 135.0f));
    }
  }
  else
  {
    ball->SetIsCollisionPaddle(false);
  }

  // 障害物との衝突判定
  for (const auto &obstacle : mObstacles)
  {
    SDL_Rect ballRect = ball->GetBallRect();
    if (obstacle->GetType() != ObstacleWindow::Type::Transparent && obstacle->CheckWindowCollision(&ballRect))
    {
      Vector2 ballPos = ball->GetWorldPos();
      Vector2 obstaclePos = obstacle->GetWindowPos();
      Vector2 obstacleSize = obstacle->GetWindowSize();

      // 上下の衝突判定
      if (ballPos.y < obstaclePos.y + ball->GetBallSize() / 2.0f ||
          ballPos.y > obstaclePos.y + obstacleSize.y - ball->GetBallSize() / 2.0f)
      {
        if (!ball->GetIsCollisionY())
        {
          ball->ReverseVelocityY();
          ball->SetIsCollisionY(true);
        }
      }
      else
      {
        ball->SetIsCollisionY(false);
      }

      // 左右の衝突判定
      if (ballPos.x < obstaclePos.x + ball->GetBallSize() / 2.0f ||
          ballPos.x > obstaclePos.x + obstacleSize.x - ball->GetBallSize() / 2.0f)
      {
        if (!ball->GetIsCollisionX())
        {
          ball->ReverseVelocityX();
          ball->SetIsCollisionX(true);
        }
      }
      else
      {
        ball->SetIsCollisionX(false);
      }

      if (gGameInstance && gGameInstance->GetSoundEffect())
      {
        Mix_PlayChannel(-1, gGameInstance->GetSoundEffect(), 0);
      }
    }
    else
    {
      ball->SetIsCollisionX(false);
      ball->SetIsCollisionY(false);
    }
  }
}

void GamePlayScene::AddBall(Vector2 pos, Vector2 velocity)
{
  try
  {
    auto newBall = std::make_unique<Ball>(pos, Vector2(mWindowSize, mWindowSize), BALL_SIZE, mMasterWindow->GetOffSetY());

    if (!newBall)
    {
      throw std::runtime_error("Failed to create new ball");
    }

    newBall->SetVelocity(velocity);
    mBalls.push_back(std::move(newBall));
    std::cout << "Debug: New ball added. Total balls: " << mBalls.size() << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error in AddBall: " << e.what() << std::endl;
  }
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
  if (mPaddle)
  {
    SDL_Rect paddleRect = mPaddle->GetPaddleRect();
    if (SDL_HasIntersection(&newRect, &paddleRect))
      return true;
  }

  // ボールとの衝突チェック
  for (const auto &ball : mBalls)
  {
    if (ball)
    { // nullチェックを追加
      SDL_Rect ballRect = ball->GetBallRect();
      if (SDL_HasIntersection(&newRect, &ballRect))
        return true;
    }
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
      mRestartButton->HandleClick(mLocalMousePos);
      mReturnToStartButton->HandleClick(mLocalMousePos);
    }
    break;

  case SDL_MOUSEBUTTONUP:
    if (event.button.button == SDL_BUTTON_LEFT)
    {
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
