#include "SceneManager.h"

void SceneManager::ChangeScene(std::unique_ptr<Scene> newScene)
{
  mCurrentScene = std::move(newScene);
}

void SceneManager::HandleInput(const Uint8 *keyState)
{
  if (mCurrentScene)
  {
    mCurrentScene->HandleInput(keyState);
  }
}

void SceneManager::Update(float deltaTime)
{
  if (mCurrentScene)
  {
    mCurrentScene->Update(deltaTime);
  }
}

void SceneManager::Render()
{
  if (mCurrentScene)
  {
    mCurrentScene->Render();
  }
}
