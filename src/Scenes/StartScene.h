#pragma once
#include "Scene.h"
#include "../GameObjects/MasterWindow.h"
#include "../Utils/TextRenderer.h"
#include "../Utils/Vector2.h"
#include "SceneManager.h"
#include "GamePlayScene.h"

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
  std::unique_ptr<TextRenderer> mPixelifySansRenderer;
  std::unique_ptr<Vector2> mScreen;
};
