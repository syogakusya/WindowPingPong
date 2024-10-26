#include "StartScene.h"

StartScene::StartScene(SDL_Renderer *renderer, TextRenderer *textRenderer)
    : mRenderer(renderer), mTextRenderer(textRenderer)
{
}

void StartScene::HandleInput(const Uint8 *keyState)
{
  if (keyState[SDL_SCANCODE_RETURN]) // Enterキーでゲーム開始
  {
    // シーンをゲームプレイに切り替える
    // ここでSceneManagerを使ってシーンを切り替える
  }
}

void StartScene::Update(float deltaTime)
{
  // スタート画面の更新ロジック
}

void StartScene::Render()
{
  SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
  SDL_RenderClear(mRenderer);

  SDL_Color textColor = {255, 255, 255, 255};
  mTextRenderer->RenderText("Press Enter to Start", 100, 100, textColor, mRenderer);

  SDL_RenderPresent(mRenderer);
}
