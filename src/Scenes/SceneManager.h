#pragma once
#include <memory>
#include "Scene.h"

class SceneManager
{
public:
  void ChangeScene(std::unique_ptr<Scene> newScene);
  void HandleInput(const Uint8 *keyState);
  void Update(float deltaTime);
  void Render();

private:
  std::unique_ptr<Scene> mCurrentScene;
};
