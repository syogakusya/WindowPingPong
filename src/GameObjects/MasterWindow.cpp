#include "MasterWindow.h"

MasterWindow::MasterWindow(
    const char *windowName, Vector2 pos, Vector2 size, int margin, Uint32 windowFlags)
    : GameObject(windowName, pos, size, windowFlags)
{
    mMargin = margin;
    int x_, y_;
    SDL_GetWindowPosition(mWindow, &x_, &y_);
    mWindowOffsetY = y_ + size.y;
}

void MasterWindow::Update(float deltaTime)
{
    UpdateWindowPosition();
}

void MasterWindow::Draw(SDL_Renderer *renderer)
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

void MasterWindow::RenderPresent(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
}
