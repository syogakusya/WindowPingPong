#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <SDL_mixer.h>
#include "Scene.h"
#include "SceneManager.h"
#include "StartScene.h"
#include "../GameObjects/Ball.h"
#include "../GameObjects/Paddle.h"
#include "../GameObjects/MasterWindow.h"
#include "../Utils/TextRenderer.h"
#include "../UI/Button.h"
#include "../GameObjects/ObstacleWindow.h"
#include "../Utils/HighScoreManager.h"
#include "../Utils/Vector2.h"

class GamePlayScene : public Scene
{
public:
  GamePlayScene();
  void Initialize() override;
  void HandleInput(const Uint8 *keyBoardState) override;
  void Update(float deltaTime) override;
  void Draw() override;
  void Shutdown() override;
  void HandleEvent(const SDL_Event &event) override;
  void CheckBallCollisions(Ball *ball);
  void SpawnObstacle();
  void UpdateObstacles(float deltaTime);
  bool CheckObstacleCollision(const Vector2 &pos) const;

private:
  void AddBall(Vector2 pos, Vector2 velocity);

  std::unique_ptr<MasterWindow> mMasterWindow;
  std::unique_ptr<Ball> mBall;
  std::vector<std::unique_ptr<Ball>> mBalls;
  std::unique_ptr<Paddle> mPaddle;
  std::unique_ptr<Vector2> mScreen;
  std::unique_ptr<TextRenderer> mPixelifySansRenderer;

  bool mPrevSpaceKeyState;
  int mScore;
  int mWindowSize;

  enum class GameState
  {
    Start,
    Playing,
    Pause,
    GameOver,
    GameClear
  };

  GameState mCurrentState = GameState::Start;

  std::unique_ptr<Button> mRestartButton;
  Vector2 mLocalMousePos;
  Vector2 mWorldMousePos;
  Vector2 mLastWorldMousePos;
  bool mMousePressed;

  std::vector<std::unique_ptr<ObstacleWindow>> mObstacles;
  float mObstacleSpawnTimer;

  HighScoreManager mHighScoreManager;
  bool mIsGameOver;

  std::unique_ptr<Button> mReturnToStartButton;

  // サウンド関連
  Mix_Music *mBGM;
  Mix_Chunk *mPaddleHitSound;
  Mix_Chunk *mObstacleHitSound;
  Mix_Chunk *mGameOverSound;
  Mix_Chunk *mButtonClickSound;
};
