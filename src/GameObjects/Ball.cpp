#include "Ball.h"

Ball::Ball(Vector2 pos, Vector2 size, int ballSize, int offSetY)
    : GameObject("Ball", pos, size, SDL_WINDOW_ALWAYS_ON_TOP),
      mBallSize(ballSize), mOffSetY(offSetY), mLastMoveDistance(0.0f, 0.0f)
{
  mVelocity = Vector2(200.0f, 235.0f);
}

void Ball::Update(float deltaTime)
{
  // mLastMoveDistance = mVelocity * deltaTime;
  mWorldPos.x += mVelocity.x * deltaTime;
  mWorldPos.y += mVelocity.y * deltaTime;
  mWindowPos.x = mWorldPos.x - mWindowSize.x / 2.0f;
  mWindowPos.y = mWorldPos.y - mWindowSize.y / 2.0f;
  ClampBallPosition();
  ClampWindowPosition();
  UpdateWindowPosition();
}

void Ball::Draw(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect ball = {static_cast<int>(mLocalPos.x - mBallSize / 2.0f),
                   static_cast<int>(mLocalPos.y - mBallSize / 2.0f),
                   static_cast<int>(mBallSize),
                   static_cast<int>(mBallSize)};
  SDL_RenderFillRect(renderer, &ball);
}

void Ball::ClampWindowPosition()
{
  // 左
  if (mWindowPos.x < 0.0f)
  {
    mWindowPos.x = 0.0f;
  }

  // 右
  else if (mWindowPos.x + mWindowSize.x > GameObject::mScreenSize.x)
  {
    mWindowPos.x = GameObject::mScreenSize.x - mWindowSize.x;
  }

  // 上
  if (mWindowPos.y < mOffSetY)
  {
    mWindowPos.y = mOffSetY;
  }

  // 下
  else if (mWindowPos.y + mWindowSize.y > GameObject::mScreenSize.y)
  {
    mWindowPos.y = GameObject::mScreenSize.y - mWindowSize.y;
  }
}

void Ball::ClampBallPosition()
{
  if (mWorldPos.x > GameObject::mScreenSize.x - mBallSize / 2.0f && mVelocity.x > 0.0f) // 右壁
  {
    mWorldPos.x = GameObject::mScreenSize.x - mBallSize / 2.0f;
    mVelocity.x *= -1.0f;
  }
  else if (mWorldPos.x < mBallSize / 2.0f && mVelocity.x < 0.0f) // 左壁
  {
    mWorldPos.x = mBallSize / 2.0f;
    mVelocity.x *= -1.0f;
  }

  if (mWorldPos.y > GameObject::mScreenSize.y - mBallSize / 2.0f && mVelocity.y > 0.0f) // 下壁
  {
    mWorldPos.y = GameObject::mScreenSize.y - mBallSize / 2.0f;
    mVelocity.y *= -1.0f;
  }
  else if (mWorldPos.y < mBallSize / 2.0f + mOffSetY && mVelocity.y < 0.0f) // 上壁
  {
    mWorldPos.y = mBallSize / 2.0f + mOffSetY;
    mVelocity.y *= -1.0f;
  }
}

SDL_Rect Ball::GetBallRect() const
{
  return {static_cast<int>(mWorldPos.x - mBallSize / 2.0f),
          static_cast<int>(mWorldPos.y - mBallSize / 2.0f),
          static_cast<int>(mBallSize),
          static_cast<int>(mBallSize)};
}
bool Ball::CheckBallCollision(SDL_Rect *other) const
{
  SDL_Rect ballRect = GetBallRect();
  return SDL_HasIntersection(&ballRect, other);
}
