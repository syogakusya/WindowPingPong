#pragma once
#include "../Utils/Vector2.h"
#include <SDL.h>
#include <functional>
#include <random>

class GameObject
{
public:
  static Vector2 mScreenSize;
  GameObject(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags);
  virtual ~GameObject();

  virtual void Update(float deltaTime) = 0;
  virtual void Draw(SDL_Renderer *renderer) = 0;
  virtual SDL_Rect GetWindowRect() const;

  void RenderPresent(SDL_Renderer *renderer);
  void UpdateWindowPosition();

  SDL_Window *GetWindow() const { return mWindow; }
  SDL_Renderer *GetRenderer() const { return mRenderer; }

  Vector2 GetWorldPos() { return mWorldPos; }
  Vector2 GetLocalPos() { return mLocalPos; }
  Vector2 GetVelocity() { return mVelocity; }
  Vector2 GetWindowSize() { return mWindowSize; }
  Vector2 GetWindowPos() { return mWindowPos; }

  void SetWorldPos(Vector2 pos)
  {
    mWorldPos = pos;
    UpdateWindowPosition();
  }
  void SetWindowPos(Vector2 pos)
  {
    mWindowPos = pos;
    UpdateLocalPos();
  }
  void SetLocalPos(Vector2 pos) { mLocalPos = pos; }
  void SetVelocity(Vector2 vel) { mVelocity = vel; }

  // ウィンドウ表示関係
  void ShowWindow();
  void HideWindow();
  bool IsWindowVisible();

  // シェイクエフェクト
  void StartShake(float duration, float intensity);

  // 衝突判定
  bool CheckWindowCollision(SDL_Rect *other) const;

protected:
  Vector2 mWorldPos;
  Vector2 mLocalPos;
  Vector2 mVelocity;
  Vector2 mWindowSize;
  Vector2 mWindowPos;
  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;

  void UpdateLocalPos();

  // シェイクエフェクト
  bool mIsShaking;
  float mShakeTime;
  float mShakeIntensity;
  Vector2 mShakeOffset;
  Vector2 mOriginalWindowPos;
  Vector2 mOriginalWorldPos;

  void UpdateShakeEffect(float deltaTime);

  std::mt19937 mRandomEngine;
  std::uniform_real_distribution<float> mRandomDistribution;
};
