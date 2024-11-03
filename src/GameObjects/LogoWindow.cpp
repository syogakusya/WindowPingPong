#include "LogoWindow.h"

LogoWindow::LogoWindow(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags)
    : GameObject(windowName, pos, size, windowFlags)
{
  // 明示的にウィンドウ位置を設定
  SDL_SetWindowPosition(mWindow,
                        static_cast<int>(pos.x - size.x / 2),
                        static_cast<int>(pos.y - size.y / 2));
}

void LogoWindow::Update(float deltaTime)
{
  int paddleX, paddleY;

  SDL_GetWindowPosition(mWindow, &paddleX, &paddleY);
  mWindowPos = Vector2(static_cast<float>(paddleX), static_cast<float>(paddleY));
  mWorldPos = mWindowPos + mLocalPos;
  UpdateShakeEffect(deltaTime);
  UpdateWindowPosition();
}

void LogoWindow::Draw(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

const char *LogoWindow::GetTitle() const
{
  return SDL_GetWindowTitle(mWindow);
}
