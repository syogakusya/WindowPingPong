#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>
#include <string>
#include "GameObjects/Ball.h"
#include "GameObjects/Paddle.h"
#include "GameObjects/MasterWindow.h"
#include "Utils/TextRenderer.h"
#include "Scenes/SceneManager.h"

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
  std::unique_ptr<TextRenderer> mPixelifySansRenderer;
  bool mIsRunning;
  Uint32 mTicksCount;
  const Uint8 *mKeyboardState;
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
};
