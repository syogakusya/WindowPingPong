#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>
#include <string>
#include "Scene.h"
#include "../GameObjects/Ball.h"
#include "../GameObjects/Paddle.h"
#include "../GameObjects/MasterWindow.h"
#include "../Utils/TextRenderer.h"
#include "../UI/Button.h"

class GamePlayScene : public Scene
{
public:
  GamePlayScene();
  void Initialize() override;
  void HandleInput(const Uint8 *keyBoardState) override;
  void Update(float deltaTime) override;
  void Draw() override;
  void Shutdown() override;

private:
  void CheckCollisions(std::unique_ptr<Ball> &ball);
  void AddBall(Vector2 pos, Vector2 velocity);

  std::unique_ptr<MasterWindow> mMasterWindow;
  std::unique_ptr<Ball> mBall;
  std::vector<std::unique_ptr<Ball>> mBalls;
  std::unique_ptr<Paddle> mPaddle;
  std::unique_ptr<Vector2> mScreen;
  std::unique_ptr<TextRenderer> mPixelifySansRenderer;

  bool mPrevSpaceKeyState;
  bool isBallCollision;
  int mScore;

  enum class GameState
  {
    Start,
    Playing,
    Pause,
    GameOver,
    GameClear
  };

  GameState mCurrentState;

  std::unique_ptr<Button> mRestartButton;
  Vector2 mLastMousePos;
  bool mMousePressed;
};
