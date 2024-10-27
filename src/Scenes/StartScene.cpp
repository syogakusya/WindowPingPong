#include "StartScene.h"
#include "SceneManager.h"

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
}

void StartScene::Render()
{
  mMasterWindow->Draw(mMasterWindow->GetRenderer());

  SDL_Color textColor = {255, 255, 255, 255};
  mPixelifySansRenderer->RenderText(
      "Press Enter to Start",
      mScreen->x / 2 - 100,
      MASTER_WINDOW_HEIGHT / 2 - 12,
      textColor,
      mMasterWindow->GetRenderer());

  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());
}

void StartScene::Shutdown()
{
  mPixelifySansRenderer.reset();
  mMasterWindow.reset();
  mScreen.reset();
}
