#pragma once
#include "GameObject.h"

class MasterWindow : public GameObject
{
public:
    MasterWindow(const char* windowName, int x, int y, int width, int height, Uint32 windowFlags);
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;
};
