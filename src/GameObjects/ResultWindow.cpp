#include "ResultWindow.h"

ResultWindow::ResultWindow(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags)
    : GameObject(windowName, pos, size, windowFlags)
{
}

void ResultWindow::Update(float deltaTime)
{
  UpdateWindowPosition();
}

void ResultWindow::Draw(SDL_Renderer *renderer)
{
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_Rect background{
      mMargin, mMargin,
      static_cast<int>(mWindowSize.x - mMargin * 2),
      static_cast<int>(mWindowSize.y - mMargin * 2)};
  SDL_RenderFillRect(renderer, &background);
}
