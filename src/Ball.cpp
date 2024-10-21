#include "Ball.h"

Ball::Ball(int x, int y, int size)
    : GameObject("Ball", x, y, size, size, SDL_WINDOW_ALWAYS_ON_TOP),
      mVelocity{200.0f, 235.0f}
{
}

void Ball::Update(float deltaTime)
{
  mWorldPos.x += mVelocity.x * deltaTime;
  mWorldPos.y += mVelocity.y * deltaTime;
  UpdateWindowPosition();
}

void Ball::Draw(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect ball{
      static_cast<int>(mLocalPos.x - mSize.x / 2.0f),
      static_cast<int>(mLocalPos.y - mSize.y / 2.0f),
      static_cast<int>(mSize.x),
      static_cast<int>(mSize.y)};
  SDL_RenderFillRect(renderer, &ball);
}
