#include "ObstacleWindow.h"
#include <iostream>

ObstacleWindow::ObstacleWindow(
    const char *windowName,
    Vector2 pos,
    Vector2 size,
    Type type,
    float duration,
    int closeCount)
    : GameObject(windowName, pos, size,
                 (type == Type::Transparent ? SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP : 0)),
      mType(type),
      mLifeTime(duration),
      mDuration(duration),
      mIsActive(true),
      mCloseCount(closeCount),
      mCurrentCloseCount(0)
{
  if (mType == Type::Popup)
  {
    SDL_SetWindowHitTest(mWindow, nullptr, nullptr);
  }
}

void ObstacleWindow::Update(float deltaTime)
{
  if (!mIsActive)
    return;

  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_WINDOWEVENT &&
        event.window.windowID == SDL_GetWindowID(mWindow))
    {
      if (event.window.event == SDL_WINDOWEVENT_CLOSE)
      {
        HandleClick();
      }
    }
  }

  if (mType == Type::Transparent)
  {
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f)
    {
      mIsActive = false;
    }
  }
  UpdateShakeEffect(deltaTime);
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
  std::cout << "HandleClick" << std::endl;
  if (mType == Type::Popup)
  {
    mCurrentCloseCount++;
    if (mCurrentCloseCount >= mCloseCount)
    {
      mIsActive = false;
    }
  }
}