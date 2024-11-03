#pragma once
#include "Scene.h"
#include "../GameObjects/MasterWindow.h"
#include "../Utils/TextRenderer.h"
#include "../Utils/Vector2.h"
#include "SceneManager.h"
#include "GamePlayScene.h"
#include "../GameObjects/Ball.h"
#include "../GameObjects/LogoWindow.h"

class StartScene : public Scene
{
public:
  StartScene();
  void Initialize() override;
  void HandleInput(const Uint8 *keyState) override;
  void Update(float deltaTime) override;
  void Render() override;
  void Shutdown() override;

private:
  std::unique_ptr<MasterWindow> mMasterWindow;
  std::unique_ptr<Ball> mBall;
  std::unique_ptr<TextRenderer> mPixelifySansRenderer;
  std::unique_ptr<Vector2> mScreen;
  std::vector<std::unique_ptr<LogoWindow>> mLogoWindows;
  void CheckCollisions(Ball *ball, LogoWindow *logoWindow);
  bool IsColliding(
      const Vector2 &ballPos, const float &ballSize,
      const Vector2 &logoPos, const Vector2 &logoWindowSize);

  bool prevBallReverseX = false;
  bool prevBallReverseY = false;
};
