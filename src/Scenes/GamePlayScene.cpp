#include "GamePlayScene.h"

using namespace std;

const int MASTER_WINDOW_HEIGHT = 160;
const int BALL_SIZE = 16;
const int PADDLE_WIDTH = 16;
const int PADDLE_HEIGHT = 100;
const float PADDLE_SPEED = 300.0f;
const int UI_MARGIN = 6;
const float MAX_BALL_SPEED = 500.0f;
const float OBSTACLE_SPAWN_INTERVAL = 7.0f; // 障害物生成間隔

GamePlayScene::GamePlayScene()
    : mScore(0),
      mObstacleSpawnTimer(OBSTACLE_SPAWN_INTERVAL),
      mHighScoreManager("build/highscore.txt"),
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

  // BGMのロード
  mBGM = Mix_LoadMUS("assets/sounds/iwashiro_cook_lu_no_chousenjo.mp3");
  if (!mBGM)
  {
    SDL_Log("BGMのロードに失敗しました: %s", Mix_GetError());
  }

  // 効果音のロード
  mPaddleHitSound = Mix_LoadWAV("assets/sounds/byau.wav");
  if (!mPaddleHitSound)
  {
    SDL_Log("パドル衝突音のロードに失敗しました: %s", Mix_GetError());
  }

  mObstacleHitSound = Mix_LoadWAV("assets/sounds/garagara.wav");
  if (!mObstacleHitSound)
  {
    SDL_Log("障害物衝突音のロードに失敗しました: %s", Mix_GetError());
  }

  mGameOverSound = Mix_LoadWAV("assets/sounds/my_damage.wav");
  if (!mGameOverSound)
  {
    SDL_Log("ゲームオーバー音のロードに失敗しました: %s", Mix_GetError());
  }

  mButtonClickSound = Mix_LoadWAV("assets/sounds/decision.wav");
  if (!mButtonClickSound)
  {
    SDL_Log("ボタンクリック音のロードに失敗しました: %s", Mix_GetError());
  }

  if (mPaddleHitSound)
    Mix_VolumeChunk(mPaddleHitSound, MIX_MAX_VOLUME / 5);
  if (mObstacleHitSound)
    Mix_VolumeChunk(mObstacleHitSound, MIX_MAX_VOLUME / 7);
  if (mGameOverSound)
    Mix_VolumeChunk(mGameOverSound, MIX_MAX_VOLUME / 5);
  if (mButtonClickSound)
    Mix_VolumeChunk(mButtonClickSound, MIX_MAX_VOLUME / 5);

  // BGMの再生
  if (mBGM)
  {
    Mix_VolumeMusic(MIX_MAX_VOLUME / 12);
    if (Mix_PlayMusic(mBGM, -1) == -1)
    {
      SDL_Log("BGMの再生に失敗しました: %s", Mix_GetError());
    }
  }

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
    Mix_PlayChannel(-1, mButtonClickSound, 0);
    // スコアのリセット
    mScore = 0;

    // 既存のボールをクリア
    mBalls.clear();

    // 新しいボールの追加
    Vector2 ballVelocity(-120.0f, 135.0f);
    Vector2 ballPos(mScreen->x / 2, mScreen->y / 2);
    AddBall(ballPos, ballVelocity);
    
    // 障害物のクリア
    mObstacles.clear();
    
    // タイマーのリセット
    mObstacleSpawnTimer = OBSTACLE_SPAWN_INTERVAL;
    
    // ゲーム状態の更新
    mCurrentState = GameState::Playing;
    mIsGameOver = false;
    
    // パドルの位置リセット
    if (mPaddle)
    {
        mPaddle->SetWorldPos(Vector2(100.0f, mScreen->y / 2));
        mPaddle->SetWindowPos(Vector2(0.0f, mScreen->y / 2 - mWindowSize / 2));
    } });

  normalColor = {100, 100, 100, 255};
  hoverColor = {150, 150, 150, 255};
  mReturnToStartButton = std::make_unique<Button>(
      "Return to Title",
      Vector2(mScreen->x / 2 + 280, 80),
      Vector2(200, 40),
      normalColor,
      hoverColor);

  mReturnToStartButton->SetOnClick([this]()
                                   {
    if (mBGM)
    {
        Mix_HaltMusic();
    }
    if (mButtonClickSound)
    {
        Mix_PlayChannel(-1, mButtonClickSound, 0);
        SDL_Delay(100);
    }
    auto startScene = std::make_unique<StartScene>();
    SceneManager::GetInstance().ChangeScene(std::move(startScene)); });
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
    static bool prevRKeyState = false;
    bool currentRKeyState = keyBoardState[SDL_SCANCODE_R];

    if (currentRKeyState && !prevRKeyState)
    {
      // リスタート処理
      mScore = 0;
      mBalls.clear();
      Vector2 ballVelocity(-120.0f, 135.0f);
      Vector2 ballPos(mScreen->x / 2, mScreen->y / 2);
      AddBall(ballPos, ballVelocity);
      mObstacles.clear();
      mObstacleSpawnTimer = OBSTACLE_SPAWN_INTERVAL;
      mCurrentState = GameState::Playing;
      mIsGameOver = false;
      if (mPaddle)
      {
        mPaddle->SetWorldPos(Vector2(100.0f, mScreen->y / 2));
        mPaddle->SetWindowPos(Vector2(0.0f, mScreen->y / 2 - mWindowSize / 2));
      }
    }
    prevRKeyState = currentRKeyState;

    static bool prevSpaceKeyState = false;
    bool currentSpaceKeyState = keyBoardState[SDL_SCANCODE_SPACE];

    if (currentSpaceKeyState && !prevSpaceKeyState)
    {
      auto startScene = std::make_unique<StartScene>();
      SceneManager::GetInstance().ChangeScene(std::move(startScene));
    }
    prevSpaceKeyState = currentSpaceKeyState;
  }
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
      Mix_PlayChannel(-1, mGameOverSound, 0);
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
        "R: Restart",
        mScreen->x / 2 - 460,
        40,
        textColor,
        mMasterWindow->GetRenderer());

    mPixelifySansRenderer->RenderText(
        "Space: Return to Title",
        mScreen->x / 2 + 240,
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

  // サウンドの解放
  if (mPaddleHitSound)
  {
    Mix_FreeChunk(mPaddleHitSound);
    mPaddleHitSound = nullptr;
  }
  if (mObstacleHitSound)
  {
    Mix_FreeChunk(mObstacleHitSound);
    mObstacleHitSound = nullptr;
  }
  if (mGameOverSound)
  {
    Mix_FreeChunk(mGameOverSound);
    mGameOverSound = nullptr;
  }
  if (mButtonClickSound)
  {
    Mix_FreeChunk(mButtonClickSound);
    mButtonClickSound = nullptr;
  }
  if (mBGM)
  {
    Mix_FreeMusic(mBGM);
    mBGM = nullptr;
  }
}

void GamePlayScene::CheckBallCollisions(Ball *ball)
{
  if (!ball)
  {
    return;
  }

  SDL_Rect paddleRect = mPaddle->GetPaddleRect();
  if (ball->CheckBallCollision(&paddleRect))
  {
    // 衝突フラグがfalseの時のみ処理を行う
    if (!ball->GetIsCollisionPaddle())
    {
      ball->SetIsCollisionPaddle(true);
      ball->ReverseVelocityX();
      mScore += 100;

      Vector2 currentVel = ball->GetVelocity();
      float currentSpeed = std::sqrt(currentVel.x * currentVel.x + currentVel.y * currentVel.y);
      if (currentSpeed < MAX_BALL_SPEED)
      {
        ball->SetVelocity(currentVel * 1.02f);
      }

      // スコアが1000点の倍数の時、パドルの位置を基準にボールを追加
      if (mScore % 1000 == 0 && mScore != 0)
      {
        Vector2 paddlePos = mPaddle->GetWorldPos();
        Vector2 screenCenter(mScreen->x / 2, mScreen->y / 2);
        Vector2 direction = screenCenter - paddlePos;
        direction.Normalize();
        paddlePos.x += direction.x * 10.0f;
        direction.x *= 200.0f; // 初期速度の大きさを設定
        direction.y *= 100.0f;

        AddBall(paddlePos, direction);
      }

      // パドルとの衝突時に効果音を再生
      Mix_PlayChannel(-1, mPaddleHitSound, 0);
    }
  }
  else
  {
    // パドルと衝突していない場合はフラグをリセット
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

      if (ball->GetIsCollisionX() || ball->GetIsCollisionY())
      {
        obstacle->StartShake(0.15f, 3.0f);
        // 障害物との衝突時に効果音を再生
        Mix_PlayChannel(-1, mObstacleHitSound, 0);
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
  }
  catch (const std::exception &e)
  {
    SDL_Log("Error in AddBall: %s", e.what());
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
      Vector2(mWindowSize, mWindowSize),
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
  {
    int x, y;
    SDL_GetGlobalMouseState(&x, &y);
    mLocalMousePos = Vector2(static_cast<float>(x), static_cast<float>(y));
    mWorldMousePos = mLocalMousePos;

    if (mCurrentState == GameState::GameOver)
    {
      mRestartButton->Update(mWorldMousePos);
      mReturnToStartButton->Update(mWorldMousePos);
    }
  }
  break;

  case SDL_MOUSEBUTTONDOWN:
    if (event.button.button == SDL_BUTTON_LEFT)
    {
      int x, y;
      SDL_GetGlobalMouseState(&x, &y);
      Vector2 mousePos(static_cast<float>(x), static_cast<float>(y));

      if (mCurrentState == GameState::GameOver)
      {
        mRestartButton->HandleClick(mousePos);
        mReturnToStartButton->HandleClick(mousePos);
      }
    }
    break;

  case SDL_WINDOWEVENT:
    // 障害物ウィンドウのイベント処理
    for (auto &obstacle : mObstacles)
    {
      if (obstacle && event.window.windowID == SDL_GetWindowID(obstacle->GetWindow()))
      {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
          obstacle->HandleClick();
        }
        break;
      }
    }
    break;
  }
}
