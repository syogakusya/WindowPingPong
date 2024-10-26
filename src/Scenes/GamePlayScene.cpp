#include "GameplayScene.h"

GamePlayScene::GamePlayScene(MasterWindow *masterWindow, Ball *ball, Paddle *paddle, TextRenderer *textRenderer)
    : mMasterWindow(masterWindow), mBall(ball), mPaddle(paddle), mTextRenderer(textRenderer), mScore(0)
{
}

void GamePlayScene::HandleInput(const Uint8 *keyState)
{
  // ゲームプレイ中の入力処理
}

void GamePlayScene::Update(float deltaTime)
{
  mBall->Update(deltaTime);
  mPaddle->Update(deltaTime);
  // 衝突判定やスコア更新など
}

void GamePlayScene::Render()
{
  mMasterWindow->Draw(mMasterWindow->GetRenderer());
  mBall->Draw(mBall->GetRenderer());
  mPaddle->Draw(mPaddle->GetRenderer());
  mPaddle->DrawBall(mPaddle->GetRenderer(), mBall);

  SDL_Color textColor = {255, 255, 255, 255};
  mTextRenderer->RenderText("Score: " + std::to_string(mScore), 10, 10, textColor, mMasterWindow->GetRenderer());

  mBall->RenderPresent(mBall->GetRenderer());
  mPaddle->RenderPresent(mPaddle->GetRenderer());
  mMasterWindow->RenderPresent(mMasterWindow->GetRenderer());
}
