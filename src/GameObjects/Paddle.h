#pragma once
#include "GameObject.h"
#include "Ball.h"

class Paddle : public GameObject
{
public:
  Paddle(Vector2 pos, Vector2 size, int paddleWidth, int paddleHeight, int offSetY);
  void Update(float deltaTime) override;
  void Draw(SDL_Renderer *renderer) override;
  void SetDirection(float direction);
  void DrawBall(SDL_Renderer *renderer, Ball *ball);
  void RenderPresent(SDL_Renderer *renderer) override;

private:
  float mDirection;
  float mSpeed;
  int mPaddleWidth;
  int mPaddleHeight;
  int mOffSetY;

  void ClampWindowPosition();
  void ClampPaddlePosition();
};
