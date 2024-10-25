#include "GameObject.h"

Vector2 GameObject::mScreenSize = Vector2(0, 0);

GameObject::GameObject(
    const char *windowName,
    Vector2 pos, Vector2 size, Uint32 windowFlags)
    : mWorldPos{static_cast<float>(pos.x), static_cast<float>(pos.y)},
      mLocalPos{static_cast<float>(size.x / 2),
                static_cast<float>(size.y / 2)},
      mVelocity{0.0f, 0.0f},
      mWindowSize{static_cast<float>(size.x),
                  static_cast<float>(size.y)},
      mWindowPos{
          static_cast<float>(pos.x - size.x / 2),
          static_cast<float>(pos.y - size.y / 2)}
{
  mWindow = SDL_CreateWindow(
      windowName,
      mWindowPos.x,
      mWindowPos.y,
      mWindowSize.x,
      mWindowSize.y,
      windowFlags);
  if (!mWindow)
  {
    SDL_Log("ウィンドウの作成に失敗しました: %s", SDL_GetError());
    return;
  }

  mRenderer = SDL_CreateRenderer(
      mWindow, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

// ローカル座標を再計算し、ウィンドウの位置を再設定する
void GameObject::UpdateWindowPosition()
{
  UpdateLocalPos();
  SDL_SetWindowPosition(mWindow,
                        static_cast<int>(mWindowPos.x),
                        static_cast<int>(mWindowPos.y));
}

void GameObject::UpdateLocalPos()
{
  mLocalPos = mWorldPos - mWindowPos;
}
