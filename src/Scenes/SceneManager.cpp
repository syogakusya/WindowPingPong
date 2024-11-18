#include "SceneManager.h"

void SceneManager::ChangeScene(std::unique_ptr<Scene> newScene)
{
  SDL_Log("シーン変更を開始します");

  if (mCurrentScene)
  {
    SDL_Log("現在のシーンをシャットダウンします");
    mCurrentScene->Shutdown();
  }

  SDL_Log("新しいシーンに切り替えます");
  mCurrentScene = std::move(newScene);

  if (mCurrentScene)
  {
    SDL_Log("新しいシーンを初期化します");
    mCurrentScene->Initialize();
  }

  SDL_Log("シーン変更が完了しました");
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

void SceneManager::HandleEvent(const SDL_Event &event)
{
  if (mCurrentScene)
  {
    mCurrentScene->HandleEvent(event);
  }
}
