#pragma once
#include "GameObject.h"

class ResultWindow : public GameObject
{
public:
  ResultWindow(const char *windowName, Vector2 pos, Vector2 size, Uint32 windowFlags);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;

private:
  int mMargin;
};
