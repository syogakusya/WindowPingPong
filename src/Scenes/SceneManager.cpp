#include "SceneManager.h"

void SceneManager::ChangeScene(std::unique_ptr<Scene> newScene)
{
  if (mCurrentScene)
  {
    mCurrentScene->Shutdown();
  }
  mCurrentScene = std::move(newScene);
  if (mCurrentScene)
  {
    mCurrentScene->Initialize();
  }
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

void SceneManager::Draw()
{
  if (mCurrentScene)
  {
    mCurrentScene->Draw();
  }
}

void SceneManager::Shutdown()
{
  if (mCurrentScene)
  {
    mCurrentScene->Shutdown();
    mCurrentScene.reset();
  }
}
