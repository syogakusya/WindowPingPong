#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include "Ball.h"
#include "Paddle.h"
#include "MasterWindow.h"

class Game
{
public:
  Game();
  bool Initialize();
  void RunLoop();
  void Shutdown();

private:
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();
  void CheckCollisions();

  std::unique_ptr<MasterWindow> mMasterWindow;
  std::unique_ptr<Ball> mBall;
  std::unique_ptr<Paddle> mPaddle;
  std::unique_ptr<Vector2> mScreen;
  bool mIsRunning;
  Uint32 mTicksCount;
  const Uint8 *mKeyboardState;
  bool mPrevSpaceKeyState;

  enum class GameState
  {
    Start,
    Playing,
    Pause,
    GameOver,
    GameClear
  };

  GameState mCurrentState;
  int mScore;

  void RenderStartScreen();
  void RenderGameOverScreen();
  void RenderScore();
};
