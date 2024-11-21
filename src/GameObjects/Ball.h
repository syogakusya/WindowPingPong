#pragma once
#include "GameObject.h"

class Ball : public GameObject
{
public:
  Ball(Vector2 pos, Vector2 size, int ballSize, int offSetY);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void ReverseVelocityX()
  {
    mVelocity.x *= -1.0f;
    mWorldPos.x -= mLastMoveDistance.x;
  }
  void ReverseVelocityY()
  {
    mVelocity.y *= -1.0f;
    mWorldPos.y -= mLastMoveDistance.y;
  }
  const Vector2 &GetVelocity() const { return mVelocity; }
  void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }
  int GetBallSize() { return mBallSize; }
  void SetBallPos(const Vector2 &pos) { mWorldPos = pos; }
  void SetBallSpeed(float speed) { mBallSpeed = speed; }
  float GetBallSpeed() { return mBallSpeed; }
  SDL_Rect GetBallRect() const;
  bool CheckBallCollision(SDL_Rect *other) const;

  bool mIsRLScreenCollision = false;

private:
  float mBallSize;
  int mOffSetY;
  float mBallSpeed;
  Vector2 mLastMoveDistance;

  void ClampWindowPosition();
  void ClampBallPosition();
};
