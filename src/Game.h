#pragma once
#include <SDL.h>
#include <vector>

struct Vector2
{
  float x;
  float y;
};

struct WindowActor
{
  Vector2 pos;
  Vector2 size;
  Vector2 screenPos;

  Vector2 GetWorldPos()
  {
    return Vector2{pos.x + screenPos.x, pos.y + screenPos.y};
  }
};

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

  std::vector<SDL_Window *> mWindows;
  std::vector<SDL_Renderer *> mRenderers;
  bool mIsRunning;
  Uint32 mTicksCount;
  const Uint8 *state;
  float mPaddleDir;

  WindowActor mScreen;
  WindowActor mPaddle;
  WindowActor mBall;
  Vector2 mBallVel;

  enum class GameState
  {
    Start,
    Playing,
    GameOver,
    GameClear
  };

  GameState mCurrentState;
  int mScore;

  void RenderStartScreen();
  void RenderGameOverScreen();
  void RenderScore();
};