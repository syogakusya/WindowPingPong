#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Scenes/SceneManager.h"
#include "Scenes/StartScene.h"

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

  bool mIsRunning;
  Uint32 mTicksCount;
  const Uint8 *mKeyboardState;
  SceneManager &mSceneManager;
};
