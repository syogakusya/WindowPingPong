#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Scenes/SceneManager.h"
#include "Scenes/StartScene.h"

class Game
{
public:
  Game();
  bool Initialize();
  void RunLoop();
  void Shutdown();
  Mix_Chunk *GetSoundEffect() const { return mSoundEffect; }

private:
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  bool mIsRunning;
  Uint32 mTicksCount;
  SceneManager &mSceneManager;

  // サウンド管理
  Mix_Music *mBGM;
  Mix_Chunk *mSoundEffect;

  void LoadAssets();
};
