#pragma once
#include "GameObject.h"

class Ball : public GameObject
{
public:
  Ball(Vector2 pos, Vector2 size, int ballSize, int offSetY);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void RenderPresent(SDL_Renderer *renderer) override;
  void ReverseVelocityX() { mVelocity.x *= -1.0f; }
  void ReverseVelocityY() { mVelocity.y *= -1.0f; }
  const Vector2 &GetVelocity() const { return mVelocity; }
  void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }
  int GetBallSize() { return mBallSize; }

private:
  float mBallSize;
  int mOffSetY;

  void ClampWindowPosition();
  void ClampBallPosition();
};
