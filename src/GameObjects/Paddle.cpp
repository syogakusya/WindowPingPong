#include "Paddle.h"

const float PADDLE_SPEED = 600.0f;

Paddle::Paddle(
    Vector2 pos,
    Vector2 size,
    int paddleWidth, int paddleHeight, int offSetY)
    : GameObject("Paddle", pos, size, SDL_WINDOW_ALWAYS_ON_TOP),
      mDirection(0.0f),
      mSpeed(PADDLE_SPEED),
      mPaddleWidth(paddleWidth),
      mPaddleHeight(paddleHeight),
      mOffSetY(offSetY),
      mIsFollowingMouse(false)
{
}

void Paddle::Update(float deltaTime)
{
    if (mIsFollowingMouse)
    {
        UpdateMouseFollow();
    }
    else
    {
        // マウスによるウィンドウ操作に対応
        int paddleX, paddleY;
        SDL_GetWindowPosition(mWindow, &paddleX, &paddleY);
        mWindowPos = Vector2(static_cast<float>(paddleX), static_cast<float>(paddleY));
        mWorldPos = mWindowPos + mLocalPos;
        mWindowPos.y = mWorldPos.y - mWindowSize.y / 2.0f;
    }

    mWorldPos.y += mDirection * mSpeed * deltaTime;
    ClampPaddlePosition();
    ClampWindowPosition();
    UpdateShakeEffect(deltaTime);
    UpdateWindowPosition();
    mDirection = 0.0f;
}

void Paddle::Draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect paddle = {static_cast<int>(mLocalPos.x - mPaddleWidth / 2.0f),
                       static_cast<int>(mLocalPos.y - mPaddleHeight / 2.0f),
                       static_cast<int>(mPaddleWidth),
                       static_cast<int>(mPaddleHeight)};
    SDL_RenderFillRect(renderer, &paddle);
}

void Paddle::DrawBall(SDL_Renderer *renderer, Ball *ball)
{
    SDL_Rect ball_ = {
        static_cast<int>(ball->GetWorldPos().x - mWindowPos.x - ball->GetBallSize() / 2.0f),
        static_cast<int>(ball->GetWorldPos().y - mWindowPos.y - ball->GetBallSize() / 2.0f),
        static_cast<int>(ball->GetBallSize()),
        static_cast<int>(ball->GetBallSize())};
    SDL_RenderFillRect(renderer, &ball_);
}

void Paddle::SetDirection(float direction)
{
    mDirection = direction;
}

void Paddle::ClampWindowPosition()
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

void Paddle::ClampPaddlePosition()
{
    float paddleTop = mWorldPos.y - mPaddleHeight / 2.0f;
    float paddleBottom = mWorldPos.y + mPaddleHeight / 2.0f;

    // 上
    if (paddleTop < mOffSetY)
    {
        mWorldPos.y = mOffSetY + mPaddleHeight / 2.0f;
    }
    // 下
    else if (paddleBottom > GameObject::mScreenSize.y)
    {
        mWorldPos.y = GameObject::mScreenSize.y - mPaddleHeight / 2.0f;
    }
}

void Paddle::UpdateMouseFollow()
{
    int mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);

    mWindowPos.x = mouseX - mWindowSize.x / 2.0f;
    mWindowPos.y = mouseY - mWindowSize.y / 2.0f;
    mWorldPos.x = mouseX;
    mWorldPos.y = mouseY;
}

void Paddle::ToggleMouseFollow()
{
    mIsFollowingMouse = !mIsFollowingMouse;
    if (mIsFollowingMouse)
    {
        // マウスをパドルの位置に移動
        SDL_WarpMouseGlobal(
            static_cast<int>(mWindowPos.x + mWindowSize.x / 2.0f),
            static_cast<int>(mWorldPos.y));
    }
}

SDL_Rect Paddle::GetPaddleRect() const
{
    return {static_cast<int>(mWorldPos.x - mPaddleWidth / 2.0f),
            static_cast<int>(mWorldPos.y - mPaddleHeight / 2.0f),
            static_cast<int>(mPaddleWidth),
            static_cast<int>(mPaddleHeight)};
}
bool Paddle::CheckPaddleCollision(SDL_Rect *other) const
{
    SDL_Rect paddleRect = GetPaddleRect();
    return SDL_HasIntersection(&paddleRect, other);
}
