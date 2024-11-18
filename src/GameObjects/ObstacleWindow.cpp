#include "ObstacleWindow.h"

ObstacleWindow::ObstacleWindow(
    const char *windowName,
    Vector2 pos,
    Vector2 size,
    Type type,
    float duration,
    int closeCount)
    : GameObject(windowName, pos, size,
                 SDL_WINDOW_ALWAYS_ON_TOP |
                     (type == Type::Transparent ? SDL_WINDOW_BORDERLESS : 0)),
      mType(type),
      mLifeTime(duration),
      mDuration(duration),
      mIsActive(true),
      mCloseCount(closeCount),
      mCurrentCloseCount(0)
{
}

void ObstacleWindow::Update(float deltaTime)
{
  if (!mIsActive)
    return;

  if (mType == Type::Transparent)
  {
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f)
    {
      mIsActive = false;
    }
  }

  UpdateWindowPosition();
}

void ObstacleWindow::Draw(SDL_Renderer *renderer)
{
  if (mType == Type::Transparent)
  {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
  }
  else
  {
    SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255);
  }
  SDL_RenderClear(renderer);
}

void ObstacleWindow::HandleClick()
{
  if (mType == Type::Popup)
  {
    mCurrentCloseCount++;
    if (mCurrentCloseCount >= mCloseCount)
    {
      mIsActive = false;
      SDL_HideWindow(mWindow);
    }
  }
}