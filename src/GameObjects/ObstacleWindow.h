#pragma once
#include "GameObject.h"
#include <functional>

class ObstacleWindow : public GameObject
{
public:
  enum class Type
  {
    Transparent, // 透明な妨害ウィンドウ
    Popup        // ポップアップ広告タイプ
  };

  ObstacleWindow(
      const char *windowName,
      Vector2 pos,
      Vector2 size,
      Type type,
      float duration,
      int closeCount = 1);

  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  bool IsExpired() const { return mType == Type::Transparent && mLifeTime <= 0.0f; }
  bool IsActive() const { return mIsActive; }
  Type GetType() const { return mType; }
  void HandleClick();
  ~ObstacleWindow()
  {
    if (mWindow)
    {
      SDL_DestroyRenderer(mRenderer);
      SDL_DestroyWindow(mWindow);
      mWindow = nullptr;
      mRenderer = nullptr;
    }
  }

private:
  Type mType;
  float mLifeTime;
  float mDuration;
  bool mIsActive;
  int mCloseCount;
  int mCurrentCloseCount;
};