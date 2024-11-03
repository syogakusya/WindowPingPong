#include "StartScene.h"

const int MASTER_WINDOW_HEIGHT = 160;
const int UI_MARGIN = 6;

StartScene::StartScene()
{
}

void StartScene::Initialize()
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
    return;
  }

  mMasterWindow = std::unique_ptr<MasterWindow>(
      new MasterWindow(
          "WindowsPingPong",
          Vector2(mScreen->x / 2, 0),
          Vector2(mScreen->x, MASTER_WINDOW_HEIGHT),
          UI_MARGIN,
          SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS));

  mPixelifySansRenderer = std::unique_ptr<TextRenderer>(
      new TextRenderer("PixelifySans-VariableFont_wght.ttf", 24));

  char title[9] = "PINGPONG";
  for (int i = 0; i < static_cast<int>(strlen(title)); ++i) // 5つのLogoWindowを生成する例
  {
    char title_[2] = {title[i], '\0'}; // ヌル文字を加
    float posX = mScreen->x / 9 + mScreen->x / 9 * i;
    float posY = mScreen->y / 2 + std::sin(i * 5) * 100;
    auto logoWindow = std::make_unique<LogoWindow>(
        std::string(title_).c_str(),
        Vector2(posX, posY),
        Vector2(200, 200),
        SDL_WINDOW_ALWAYS_ON_TOP);
    mLogoWindows.push_back(std::move(logoWindow));
  }

  Vector2 ballVelocity(300.0f, 300.0f);
  mBall = std::unique_ptr<Ball>(
      new Ball(
          Vector2(mScreen->x / 2, mScreen->y / 2 + 200),
          Vector2(200, 200),
          20,
          mMasterWindow->GetOffSetY()));
  mBall->SetVelocity(ballVelocity);
}

void StartScene::HandleInput(const Uint8 *keyState)
{
  if (keyState[SDL_SCANCODE_RETURN])
  {
    auto gamePlayScene = std::make_unique<GamePlayScene>();
    SceneManager::GetInstance().ChangeScene(std::move(gamePlayScene));
  }
}

void StartScene::Update(float deltaTime)
{
  mMasterWindow->Update(deltaTime);
  mBall->Update(deltaTime);

  // 各LogoWindowとボールの衝突判定
  for (const auto &logoWindow : mLogoWindows)
  {
    CheckCollisions(mBall.get(), logoWindow.get());
    logoWindow->Update(deltaTime);
  }
}

void StartScene::CheckCollisions(Ball *ball, LogoWindow *logoWindow)
{
  Vector2 ballPos = ball->GetWorldPos();
  Vector2 logoWindowSize = logoWindow->GetWindowSize();
  Vector2 logoPos = logoWindow->GetWindowPos();
  float ballSize = ball->GetBallSize();

  if (IsColliding(ballPos, ballSize, logoPos, logoWindowSize))
  {
    // シェイクエフェクトを開始
    logoWindow->StartShake(0.15f, 5.0f);
    // 上下の衝突
    if (ballPos.y < logoPos.y || ballPos.y > logoPos.y + logoWindowSize.y)
    {
      if (!prevBallReverseY)
      {
        ball->ReverseVelocityY();
      }
      prevBallReverseY = true;
    }
    else
    {
      prevBallReverseY = false;
    }

    // 左右の衝突
    if (ballPos.x < logoPos.x || ballPos.x > logoPos.x + logoWindowSize.x)
    {
      if (!prevBallReverseX)
      {
        ball->ReverseVelocityX();
      }
      prevBallReverseX = true;
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

bool StartScene::IsColliding(
    const Vector2 &ballPos, const float &ballSize,
    const Vector2 &logoPos, const Vector2 &logoWindowSize)
{
  return (ballPos.x + ballSize / 2.0f > logoPos.x &&                    // 右
          ballPos.x - ballSize / 2.0f < logoPos.x + logoWindowSize.x && // 左
          ballPos.y + ballSize / 2.0f > logoPos.y &&                    // 下
          ballPos.y - ballSize / 2.0f < logoPos.y + logoWindowSize.y);  // 上
}

void StartScene::Render()
{
  mBall->Draw(mBall->GetRenderer());

  SDL_Color textColor = {255, 255, 255, 255};
  mPixelifySansRenderer->SetFontSize(120);
  for (const auto &logoWindow : mLogoWindows)
  {
    logoWindow->Draw(logoWindow->GetRenderer());
    Vector2 windowSize = logoWindow->GetWindowSize();
    Vector2 windowPos = logoWindow->GetWindowPos();
    mPixelifySansRenderer->RenderText(
        logoWindow->GetTitle(),
        windowSize.x / 2 - 40,
        windowSize.y / 2 - 60,
        textColor,
        logoWindow->GetRenderer());
    logoWindow->RenderPresent(logoWindow->GetRenderer());

    Vector2 ballWindowPos = mBall->GetWindowPos();
    mPixelifySansRenderer->RenderText(
        logoWindow->GetTitle(),
        windowSize.x / 2 - 40 + windowPos.x - ballWindowPos.x,
        windowSize.y / 2 - 60 + windowPos.y - ballWindowPos.y,
        textColor,
        mBall->GetRenderer());
  }

  mPixelifySansRenderer->SetFontSize(24);
  mMasterWindow->Draw(mMasterWindow->GetRenderer());
  mPixelifySansRenderer->RenderText(
      "Press Enter to Start",
      mScreen->x / 2 - 160,
      MASTER_WINDOW_HEIGHT / 2 - 12,
      textColor,
      mMasterWindow->GetRenderer());

  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());

  // ボールの描画
  mBall->RenderPresent(mBall->GetRenderer());
}

void StartScene::Shutdown()
{
  mPixelifySansRenderer.reset();
  mMasterWindow.reset();
  mScreen.reset();
  mBall.reset();
  for (auto &logoWindow : mLogoWindows)
  {
    logoWindow.reset();
  }
  mLogoWindows.clear();
}
