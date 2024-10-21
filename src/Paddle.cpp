#include "Paddle.h"

const float PADDLE_SPEED = 300.0f;

Paddle::Paddle(int x, int y, int width, int height)
    : GameObject("Paddle", x, y, width, height, SDL_WINDOW_ALWAYS_ON_TOP),
      mDirection(0.0f),
      mSpeed(PADDLE_SPEED)
{
}

void Paddle::Update(float deltaTime)
{
    mWorldPos.y += mDirection * mSpeed * deltaTime;
    UpdateWindowPosition();
    mDirection = 0.0f;  // Reset direction after update
}

void Paddle::Draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect paddle{
        static_cast<int>(mLocalPos.x - mSize.x / 2.0f),
        static_cast<int>(mLocalPos.y - mSize.y / 2.0f),
        static_cast<int>(mSize.x),
        static_cast<int>(mSize.y)
    };
    SDL_RenderFillRect(renderer, &paddle);
}

void Paddle::SetDirection(float direction)
{
    mDirection = direction;
}
