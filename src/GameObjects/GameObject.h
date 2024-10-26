#pragma once
#include "../Utils/Vector2.h"
#include <SDL.h>
#include <functional>

class GameObject
{
public:
  static Vector2 mScreenSize;
  GameObject(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags);
  virtual ~GameObject();

  virtual void Update(float deltaTime) = 0;
  virtual void Draw(SDL_Renderer *renderer) = 0;
  virtual void RenderPresent(SDL_Renderer *renderer) = 0;

  void UpdateWindowPosition();

  SDL_Window *GetWindow() const { return mWindow; }
  SDL_Renderer *GetRenderer() const { return mRenderer; }

  Vector2 GetWorldPos() const { return mWorldPos; }
  Vector2 GetLocalPos() const { return mLocalPos; }
  Vector2 GetVelocity() const { return mVelocity; }
  Vector2 GetWindowSize() const { return mWindowSize; }
  Vector2 GetWindowPos() const { return mWindowPos; }

  // ウィンドウ表示関係
  void ShowWindow();
  void HideWindow();
  bool IsWindowVisible() const;

protected:
  Vector2 mWorldPos;
  Vector2 mLocalPos;
  Vector2 mVelocity;
  Vector2 mWindowSize;
  Vector2 mWindowPos;
  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;

  void UpdateLocalPos();
};
