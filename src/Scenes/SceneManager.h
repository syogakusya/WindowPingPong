#pragma once
#include <memory>
#include "Scene.h"

class SceneManager
{
public:
  static SceneManager &GetInstance()
  {
    static SceneManager instance;
    return instance;
  }

  SceneManager() = default;
  ~SceneManager() = default;
  SceneManager(const SceneManager &) = delete;
  SceneManager &operator=(const SceneManager &) = delete;

  void ChangeScene(std::unique_ptr<Scene> newScene);
  void HandleInput(const Uint8 *keyState);
  void HandleEvent(const SDL_Event &event);
  void Update(float deltaTime);
  void Draw();
  void Shutdown();

private:
  std::unique_ptr<Scene> mCurrentScene;
};
