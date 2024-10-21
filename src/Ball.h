#pragma once
#include "GameObject.h"

class Ball : public GameObject
{
public:
  Ball(int x, int y, int size);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void ReverseVelocityX() { mVelocity.x *= -1.0f; }
  void ReverseVelocityY() { mVelocity.y *= -1.0f; }

  const Vector2 &GetVelocity() const { return mVelocity; }
  void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }

private:
  Vector2 mVelocity;
};
