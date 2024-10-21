#include "MasterWindow.h"

MasterWindow::MasterWindow(const char* windowName, int x, int y, int width, int height, Uint32 windowFlags)
    : GameObject(windowName, x, y, width, height, windowFlags)
{
}

void MasterWindow::Update(float deltaTime)
{
    // マスターウィンドウの更新処理（必要に応じて実装）
}

void MasterWindow::Draw(SDL_Renderer* renderer)
{
    // マスターウィンドウの描画処理（必要に応じて実装）
}
