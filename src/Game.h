#pragma once
#include <SDL.h>
#include <vector>
#include <cmath>
#include <cstdio>

struct Vector2
{
  float x;
  float y;

  Vector2 operator+(const Vector2 &other) const
  {
    return Vector2{x + other.x, y + other.y};
  }

  Vector2 &operator+=(const Vector2 &other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2 operator-(const Vector2 &other) const
  {
    return Vector2{x - other.x, y - other.y};
  }

  Vector2 &operator-=(const Vector2 &other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }
};

struct WindowActor
{
  Vector2 pos;
  Vector2 size;
  Vector2 screenPos;

  Vector2 GetLocalPos()
  {
    return Vector2{pos.x - screenPos.x, pos.y - screenPos.y};
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
  bool prevSpaceKeyState;
  bool isBallCollision;

  WindowActor mScreen;
  WindowActor mPaddle;
  WindowActor mBall;
  WindowActor mMasterWindow;
  Vector2 mBallVel;

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
