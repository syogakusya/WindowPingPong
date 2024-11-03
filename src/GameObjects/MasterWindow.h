#pragma once
#include "GameObject.h"

class MasterWindow : public GameObject
{
public:
    MasterWindow(const char *windowName, Vector2 pos, Vector2 size, int margin, Uint32 windowFlags);
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer *renderer) override;

    int GetOffSetY() { return mWindowOffsetY; }

private:
    int mMargin;
    int mWindowOffsetY;
};
