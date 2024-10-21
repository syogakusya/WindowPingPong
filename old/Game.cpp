#include "Game.h"

const int thickness = 15;
const float paddleWidth = 100.0f;
const int masterWindowHeight = 160;
const int windowWidth = 200;
const int windowHeight = 200;
const float paddleSpeed = 600.0f;
const float ballSpeed = 300.0f;
Game::Game() : mIsRunning(true), mTicksCount(0), mPaddleDir(0.0f), isBallCollision(false), mCurrentState(GameState::Start), mScore(0)
{
}

bool Game::Initialize()
{
  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0)
  {
    SDL_Log("SDLを初期化できませんでした : %s", SDL_GetError());
    return false;
  }

  SDL_DisplayMode displayMode;
  if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
  {
    mScreen.size.x = displayMode.w;
    mScreen.size.y = displayMode.h;
    printf("mScreen.size.x: %f, mScreen.size.y: %f\n", mScreen.size.x, mScreen.size.y);
  }

  SDL_Window *masterWindow = SDL_CreateWindow("WindowsPingPong", 0, 0, mScreen.size.x, masterWindowHeight, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
  if (!masterWindow)
  {
    SDL_Log("マスターウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(masterWindow);
  int masterX, masterY;
  SDL_GetWindowPosition(masterWindow, &masterX, &masterY);
  mMasterWindow.screenPos = {static_cast<float>(masterX), static_cast<float>(masterY)};
  int masterWidth, masterHeight;
  SDL_GetWindowSize(masterWindow, &masterWidth, &masterHeight);
  mMasterWindow.size = {static_cast<float>(masterWidth), static_cast<float>(masterHeight)};

  // ボール用ウィンドウの作成
  SDL_Window *ballWindow = SDL_CreateWindow("Ball", mScreen.size.x / 2, mScreen.size.y / 2, windowWidth, windowHeight, SDL_WINDOW_ALWAYS_ON_TOP);
  if (!ballWindow)
  {
    SDL_Log("ボールウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(ballWindow);
  int ballX, ballY;
  SDL_GetWindowPosition(ballWindow, &ballX, &ballY);
  mBall.screenPos = {static_cast<float>(ballX), static_cast<float>(ballY)};
  int ballWidth, ballHeight;
  SDL_GetWindowSize(ballWindow, &ballWidth, &ballHeight);
  mBall.size = {static_cast<float>(ballWidth), static_cast<float>(ballHeight)};
  mBall.pos = {ballWidth / 2.0f + mBall.screenPos.x, ballHeight / 2.0f + mBall.screenPos.y};
  mBallVel = {-200.0f, 235.0f};

  // パドル用ウィンドウの作成
  SDL_Window *paddleWindow = SDL_CreateWindow("Paddle", thickness, mScreen.size.y / 2, windowWidth, windowHeight, SDL_WINDOW_ALWAYS_ON_TOP);
  if (!paddleWindow)
  {
    SDL_Log("パドルウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(paddleWindow);
  int paddleX, paddleY;
  SDL_GetWindowPosition(paddleWindow, &paddleX, &paddleY);
  mPaddle.screenPos = {static_cast<float>(paddleX), static_cast<float>(paddleY)};
  int paddleWidth, paddleHeight;
  SDL_GetWindowSize(paddleWindow, &paddleWidth, &paddleHeight);
  mPaddle.size = {static_cast<float>(paddleWidth), static_cast<float>(paddleHeight)};
  mPaddle.pos = {paddleWidth / 2.0f + mPaddle.screenPos.x, paddleHeight / 2.0f + mPaddle.screenPos.y};

  // レンダラーの作成
  for (auto window : mWindows)
  {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
      SDL_Log("レンダラーを作成できませんでした : %s", SDL_GetError());
      return false;
    }
    mRenderers.push_back(renderer);
  }

  mCurrentState = GameState::Playing;

  return true;
}

void Game::Shutdown()
{
  for (auto renderer : mRenderers)
  {
    SDL_DestroyRenderer(renderer);
  }
  for (auto window : mWindows)
  {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
}

void Game::RunLoop()
{
  while (mIsRunning)
  {
    ProcessInput();
    if (mCurrentState == GameState::Playing && mCurrentState != GameState::Pause)
    {
      UpdateGame();
    }
    GenerateOutput();
  }
}

void Game::ProcessInput()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_QUIT:
      mIsRunning = false;
      break;
    }
  }

  state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_ESCAPE])
  {
    mIsRunning = false;
  }

  // パドルの移動
  if ((state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) && mCurrentState == GameState::Playing)
  {
    mPaddleDir -= 1.0f;
  }
  if ((state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) && mCurrentState == GameState::Playing)
  {
    mPaddleDir += 1.0f;
  }

  if (state[SDL_SCANCODE_SPACE])
  {
    if (prevSpaceKeyState == false)
    {
      if (mCurrentState == GameState::Playing)
      {
        mCurrentState = GameState::Pause;
      }
      else if (mCurrentState == GameState::Pause)
      {
        mCurrentState = GameState::Playing;
      }
    }
    prevSpaceKeyState = true;
  }
  else
  {
    if (prevSpaceKeyState == true)
    {
      prevSpaceKeyState = false;
    }
  }

  if (state[SDL_SCANCODE_Q])
  {
    // マスターウィンドウの情報を出力
    int masterX, masterY, masterWidth, masterHeight;
    SDL_GetWindowPosition(mWindows[0], &masterX, &masterY);
    SDL_GetWindowSize(mWindows[0], &masterWidth, &masterHeight);
    printf("マスターウィンドウ - 位置: (%d, %d), サイズ: %d x %d\n", masterX, masterY, masterWidth, masterHeight);

    // パドルウィンドウの情報を出力
    int paddleX, paddleY, paddleWidth, paddleHeight;
    SDL_GetWindowPosition(mWindows[2], &paddleX, &paddleY);
    SDL_GetWindowSize(mWindows[2], &paddleWidth, &paddleHeight);
    printf("パドルウィンドウ - 位置: (%d, %d), サイズ: %d x %d\n", paddleX, paddleY, paddleWidth, paddleHeight);
  }
}

void Game::UpdateGame()
{
  // 前フレームから16ms以上経過していない場合は待機
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16.6))
    ;

  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
  mTicksCount = SDL_GetTicks();
  if (deltaTime > 0.05f)
  {
    deltaTime = 0.05f;
  }

  // マウスでのウィンドウ位置操作に対応
  int ballX, ballY, paddleX, paddleY;
  SDL_GetWindowPosition(mWindows[1], &ballX, &ballY);
  SDL_GetWindowPosition(mWindows[2], &paddleX, &paddleY);
  Vector2 ballLocalPos = mBall.GetLocalPos(), paddleLocalPos = mPaddle.GetLocalPos();
  mBall.screenPos = {static_cast<float>(ballX), static_cast<float>(ballY)};
  mPaddle.screenPos = {static_cast<float>(paddleX), static_cast<float>(paddleY)};
  mBall.pos = mBall.screenPos + ballLocalPos;
  mPaddle.pos = mPaddle.screenPos + paddleLocalPos;

  // パドルの移動
  if (mPaddleDir != 0.0f)
  {
    mPaddle.pos.y += mPaddleDir * paddleSpeed * deltaTime;
    mPaddle.screenPos.y = mPaddle.pos.y - paddleLocalPos.y;
  }
  mPaddleDir = 0.0f;

  // パドルの位置制限
  float paddleTop = mPaddle.pos.y - paddleWidth / 2.0f;
  float paddleBottom = mPaddle.pos.y + paddleWidth / 2.0f;

  if (paddleTop < mMasterWindow.screenPos.y + mMasterWindow.size.y)
  {
    mPaddle.pos.y = mMasterWindow.screenPos.y + mMasterWindow.size.y + paddleWidth / 2.0f;
  }
  else if (paddleBottom > mScreen.size.y)
  {
    mPaddle.pos.y = mScreen.size.y - paddleWidth / 2.0f;
  }

  // パドルウィンドウの位置制限
  mPaddle.screenPos.y = mPaddle.pos.y - mPaddle.size.y / 2.0f;

  // 上
  if (mPaddle.screenPos.y < mMasterWindow.screenPos.y + mMasterWindow.size.y)
  {
    mPaddle.screenPos.y = mMasterWindow.screenPos.y + mMasterWindow.size.y;
  }
  // 下
  else if (mPaddle.screenPos.y + mPaddle.size.y > mScreen.size.y)
  {
    mPaddle.screenPos.y = mScreen.size.y - mPaddle.size.y;
  }

  // ウィンドウ位置を整数値に丸める（ピクセル単位で正確に配置するため）
  mPaddle.screenPos.y = std::round(mPaddle.screenPos.y);

  mBall.pos.x += mBallVel.x * deltaTime;
  mBall.pos.y += mBallVel.y * deltaTime;
  mBall.screenPos.x = mBall.pos.x - mBall.size.x / 2.0f;
  mBall.screenPos.y = mBall.pos.y - mBall.size.y / 2.0f;

  // ボールの位置制限
  if (mBall.pos.x > mScreen.size.x - thickness / 2.0f && mBallVel.x > 0.0f) // 右壁
  {
    mBall.pos.x = mScreen.size.x - thickness / 2.0f;
    mBallVel.x *= -1.0f;
  }
  else if (mBall.pos.x < thickness / 2.0f && mBallVel.x < 0.0f) // 左壁
  {
    mBall.pos.x = thickness / 2.0f;
    mBallVel.x *= -1.0f;
  }

  if (mBall.pos.y > mScreen.size.y - thickness / 2.0f && mBallVel.y > 0.0f) // 下壁
  {
    mBall.pos.y = mScreen.size.y - thickness / 2.0f;
    mBallVel.y *= -1.0f;
  }
  else if (mBall.pos.y < thickness / 2.0f + mMasterWindow.screenPos.y + mMasterWindow.size.y && mBallVel.y < 0.0f) // 上壁
  {
    mBall.pos.y = thickness / 2.0f + mMasterWindow.screenPos.y + mMasterWindow.size.y;
    mBallVel.y *= -1.0f;
  }

  // ボールウィンドウの制限
  mBall.screenPos.x = mBall.pos.x - mBall.size.x / 2.0f;
  mBall.screenPos.y = mBall.pos.y - mBall.size.y / 2.0f;

  // 左
  if (mBall.screenPos.x < 0.0f)
  {
    mBall.screenPos.x = 0.0f;
  }
  // 右
  else if (mBall.screenPos.x + mBall.size.x > mScreen.size.x)
  {
    mBall.screenPos.x = mScreen.size.x - mBall.size.x;
  }

  // 上
  if (mBall.screenPos.y < mMasterWindow.screenPos.y + mMasterWindow.size.y)
  {
    mBall.screenPos.y = mMasterWindow.screenPos.y + mMasterWindow.size.y;
  }
  // 下
  else if (mBall.screenPos.y + mBall.size.y > mScreen.size.y)
  {
    mBall.screenPos.y = mScreen.size.y - mBall.size.y;
  }

  // ボールとパドルの衝突判定
  if (mBall.pos.y - thickness / 2.0f < mPaddle.pos.y + paddleWidth / 2.0f && mBall.pos.y + thickness / 2.0f > mPaddle.pos.y - paddleWidth / 2.0f &&
      mPaddle.pos.x - thickness / 2.0f < mBall.pos.x + thickness / 2.0f && mPaddle.pos.x + thickness / 2.0f > mBall.pos.x - thickness / 2.0f)
  {
    printf("検知\n");
    if (isBallCollision == false)
    {
      mBallVel.x *= -1.0f;
      printf("衝突\n");
    }
    isBallCollision = true;
  }
  else
  {
    isBallCollision = false;
  }
}

void Game::GenerateOutput()
{
  // ウィンドウ位置更新
  SDL_SetWindowPosition(mWindows[1], mBall.screenPos.x, mBall.screenPos.y);
  SDL_SetWindowPosition(mWindows[2], static_cast<int>(mPaddle.screenPos.x), static_cast<int>(mPaddle.screenPos.y));
  SDL_SetRenderDrawColor(mRenderers[0], 255, 255, 255, 255);
  SDL_RenderClear(mRenderers[0]);
  SDL_SetRenderDrawColor(mRenderers[0], 0, 0, 0, 255);
  SDL_Rect background{thickness / 2, thickness / 2, static_cast<int>(mScreen.size.x - thickness), static_cast<int>(masterWindowHeight - thickness)};
  SDL_RenderFillRect(mRenderers[0], &background);
  SDL_RenderPresent(mRenderers[0]);

  // ボールの描画
  SDL_SetRenderDrawColor(mRenderers[1], 0, 0, 0, 255);
  SDL_RenderClear(mRenderers[1]);
  SDL_SetRenderDrawColor(mRenderers[1], 255, 255, 255, 255);
  SDL_Rect ball{
      static_cast<int>(mBall.GetLocalPos().x - thickness / 2.0f),
      static_cast<int>(mBall.GetLocalPos().y - thickness / 2.0f),
      thickness,
      thickness};
  SDL_RenderFillRect(mRenderers[1], &ball);
  SDL_RenderPresent(mRenderers[1]);

  // パドルの描画
  SDL_SetRenderDrawColor(mRenderers[2], 0, 0, 0, 255);
  SDL_RenderClear(mRenderers[2]);
  SDL_SetRenderDrawColor(mRenderers[2], 255, 255, 255, 255);
  SDL_Rect paddle{
      static_cast<int>(mPaddle.GetLocalPos().x - thickness / 2.0f),
      static_cast<int>(mPaddle.GetLocalPos().y - paddleWidth / 2.0f),
      thickness,
      static_cast<int>(paddleWidth)};
  SDL_RenderFillRect(mRenderers[2], &paddle);
  SDL_Rect ball2 = {
      static_cast<int>(mBall.pos.x - mPaddle.screenPos.x - thickness / 2.0f),
      static_cast<int>(mBall.pos.y - mPaddle.screenPos.y - thickness / 2.0f),
      thickness,
      thickness};
  SDL_RenderFillRect(mRenderers[2], &ball2);
  SDL_RenderPresent(mRenderers[2]);
}
