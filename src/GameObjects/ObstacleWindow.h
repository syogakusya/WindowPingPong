#pragma once
#include "GameObject.h"
#include <SDL_image.h>
#include <string>
#include <vector>

class ObstacleWindow : public GameObject
{
public:
  enum class Type
  {
    Transparent,
    Popup
  };

  enum class AdType
  {
    Game, // ゲーム広告
    Food, // 飲食店広告
    Shop  // ショッピング広告
  };

  ObstacleWindow(
      const char *windowName,
      Vector2 pos,
      Vector2 size,
      Type type,
      float duration,
      int closeCount = 1);

  ~ObstacleWindow();
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  bool IsExpired() const { return mType == Type::Transparent && mLifeTime <= 0.0f; }
  bool IsActive() const { return mIsActive; }
  Type GetType() const { return mType; }
  void HandleClick();

private:
  void LoadAdImages();
  SDL_Texture *LoadTexture(const std::string &path);

  Type mType;
  AdType mAdType;
  float mLifeTime;
  float mDuration;
  bool mIsActive;
  int mCloseCount;
  int mCurrentCloseCount;

  std::vector<SDL_Texture *> mAdTextures;
  SDL_Texture *mCurrentTexture;
};