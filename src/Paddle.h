#pragma once
#include "GameObject.h"

class Paddle : public GameObject
{
public:
  Paddle(int x, int y, int width, int height);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void SetDirection(float direction);

private:
  float mDirection;
  float mSpeed;
};
