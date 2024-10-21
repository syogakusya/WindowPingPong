#include "GameObject.h"

GameObject::GameObject(const char *windowName, int x, int y, int width, int height, Uint32 windowFlags)
    : mWorldPos{static_cast<float>(x), static_cast<float>(y)},
      mLocalPos{0.0f, 0.0f},
      mVelocity{0.0f, 0.0f},
      mSize{static_cast<float>(width), static_cast<float>(height)},
      worldPos(mWorldPos, [this]()
               { UpdateWindowPosition(); }),
      localPos(mLocalPos, [this]()
               { UpdateLocalPos(); }),
      velocity(mVelocity, []() {})
{
  mWindow = SDL_CreateWindow(windowName, x, y, width, height, windowFlags);
  if (!mWindow)
  {
    SDL_Log("ウィンドウの作成に失敗しました: %s", SDL_GetError());
    return;
  }

  mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!mRenderer)
  {
    SDL_Log("レンダラーの作成に失敗しました: %s", SDL_GetError());
    return;
  }
}

GameObject::~GameObject()
{
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
}

void GameObject::UpdateWindowPosition()
{
  SDL_SetWindowPosition(mWindow, static_cast<int>(mWorldPos.x), static_cast<int>(mWorldPos.y));
  UpdateLocalPos();
}

void GameObject::UpdateLocalPos()
{
  mLocalPos = mWorldPos - Vector2{static_cast<float>(mSize.x / 2), static_cast<float>(mSize.y / 2)};
}
