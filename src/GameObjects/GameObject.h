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
  ~GameObject();

  virtual void Update(float deltaTime) = 0;
  virtual void Draw(SDL_Renderer *renderer) = 0;

  void RenderPresent(SDL_Renderer *renderer);
  void UpdateWindowPosition();

  SDL_Window *GetWindow() const { return mWindow; }
  SDL_Renderer *GetRenderer() const { return mRenderer; }

  // ゲッタ
  Vector2 GetWorldPos() { return mWorldPos; }
  Vector2 GetLocalPos() { return mLocalPos; }
  Vector2 GetVelocity() { return mVelocity; }
  Vector2 GetWindowSize() { return mWindowSize; }
  Vector2 GetWindowPos() { return mWindowPos; }

  // ウィンドウ表示関係
  void ShowWindow();
  void HideWindow();
  bool IsWindowVisible();

  // シェイクエフェクト
  void StartShake(float duration, float intensity);

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
