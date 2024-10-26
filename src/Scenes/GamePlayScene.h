#pragma once
#include "Scene.h"
#include "../GameObjects/Ball.h"
#include "../GameObjects/Paddle.h"
#include "../GameObjects/MasterWindow.h"
#include "../Utils/TextRenderer.h"

class GamePlayScene : public Scene
{
public:
  GamePlayScene(MasterWindow *masterWindow, Ball *ball, Paddle *paddle, TextRenderer *textRenderer);
  void HandleInput(const Uint8 *keyState) override;
  void Update(float deltaTime) override;
  void Render() override;

private:
  MasterWindow *mMasterWindow;
  Ball *mBall;
  Paddle *mPaddle;
  TextRenderer *mTextRenderer;
  int mScore;
};
