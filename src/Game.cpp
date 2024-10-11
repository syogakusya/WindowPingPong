#include "Game.h"

const int thickness = 15;
const float paddleWidth = 100.0f;
const float masterWindowHeight = 200.0f;
const int windowWidth = 200;
const int windowHeight = 200;
const float paddleSpeed = 600.0f;

Game::Game() : mIsRunning(true), mTicksCount(0), mPaddleDir(0.0f)
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
  }

  SDL_Window *masterWindow = SDL_CreateWindow("WindowsPingPong", 0, 0, mScreen.size.x, masterWindowHeight, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_BORDERLESS);
  if (!masterWindow)
  {
    SDL_Log("マスターウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(masterWindow);

  // ボール用ウィンドウの作成
  SDL_Window *ballWindow = SDL_CreateWindow("Ball", mScreen.size.x / 2, mScreen.size.y / 2, windowWidth, windowHeight, SDL_WINDOW_ALWAYS_ON_TOP);
  if (!ballWindow)
  {
    SDL_Log("ボールウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(ballWindow);
  int x, y;
  SDL_GetWindowPosition(ballWindow, &x, &y);
  mBall.screenPos.x = x;
  mBall.screenPos.y = y;
  mBall.size.x = windowWidth;
  mBall.size.y = windowHeight;

  // パドル用ウィンドウの作成
  SDL_Window *paddleWindow = SDL_CreateWindow("Paddle", thickness, mScreen.size.y / 2, windowWidth, windowHeight, SDL_WINDOW_ALWAYS_ON_TOP);
  if (!paddleWindow)
  {
    SDL_Log("パドルウィンドウを作成できませんでした : %s", SDL_GetError());
    return false;
  }
  mWindows.push_back(paddleWindow);
  SDL_GetWindowPosition(paddleWindow, &x, &y);
  mPaddle.screenPos.x = x;
  mPaddle.screenPos.y = y;
  mPaddle.size.x = windowWidth;
  mPaddle.size.y = windowHeight;

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

  mBall.pos = {windowWidth / 2.0f, windowHeight / 2.0f};
  mPaddle.pos = {windowWidth / 2.0f, windowHeight / 2.0f};
  mBallVel = {-200.0f, 235.0f};

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
    UpdateGame();
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
  if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W])
  {
    mPaddleDir -= 1.0f;
  }
  if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S])
  {
    mPaddleDir += 1.0f;
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

  // ウィンドウの位置更新
  int ballX, ballY, paddleX, paddleY;
  SDL_GetWindowPosition(mWindows[1], &ballX, &ballY);
  SDL_GetWindowPosition(mWindows[2], &paddleX, &paddleY);

  mBall.screenPos.x = static_cast<float>(ballX);
  mBall.screenPos.y = static_cast<float>(ballY);
  mPaddle.screenPos.x = paddleX;
  mPaddle.screenPos.y = paddleY;

  // パドルの移動
  if (mPaddleDir != 0.0f)
  {
    mPaddle.pos.y += mPaddleDir * paddleSpeed * deltaTime;
    // パドルの位置制限を修正
    if (mPaddle.pos.y < paddleWidth / 2.0f || mPaddle.pos.y > mPaddle.size.y - paddleWidth / 2.0f)
    {
      mPaddle.pos.y -= mPaddleDir * paddleSpeed * deltaTime;
      mPaddle.screenPos.y += mPaddleDir * paddleSpeed * deltaTime;
    }

    mPaddleDir = 0.0f;
  }

  mBall.pos.x += mBallVel.x * deltaTime;
  mBall.pos.y += mBallVel.y * deltaTime;
  if (mBall.pos.x < thickness / 2.0f)
  {
    mBall.pos.x = thickness / 2.0f;
    mBall.screenPos.x += mBallVel.x * deltaTime;
  }
  if (mBall.pos.x > mBall.size.x - thickness / 2.0f)
  {
    mBall.pos.x = mBall.size.x - thickness / 2.0f;
    mBall.screenPos.x += mBallVel.x * deltaTime;
  }

  if (mBall.pos.y < thickness / 2.0f)
  {
    mBall.pos.y = thickness / 2.0f;
    mBall.screenPos.y += mBallVel.y * deltaTime;
  }
  if (mBall.pos.y > mBall.size.y - thickness / 2.0f)
  {
    mBall.pos.y = mBall.size.y - thickness / 2.0f;
    mBall.screenPos.y += mBallVel.y * deltaTime;
  }

  // ボールとパドルの衝突判定を修正
  float diffX = mPaddle.GetWorldPos().x - mBall.GetWorldPos().x;
  float diffY = mPaddle.GetWorldPos().y - mBall.GetWorldPos().y;

  if (std::abs(diffY) <= paddleWidth / 2.0f && std::abs(diffX) <= thickness / 2.0f)
  {
    mBallVel.x *= -1.0f;
  }

  // ボールの壁反射
  if ((mBall.GetWorldPos().y <= (thickness / 2.0f) + masterWindowHeight && mBallVel.y < 0.0f) || (mBall.GetWorldPos().y >= mScreen.size.y - thickness / 2.0f && mBallVel.y > 0.0f))
  {
    mBallVel.y *= -1.0f;
  }
  if (mBall.GetWorldPos().x >= mScreen.size.x - thickness / 2.0f && mBallVel.x > 0.0f)
  {
    mBallVel.x *= -1.0f;
  }
  if (mBall.GetWorldPos().x <= thickness / 2.0f && mBallVel.x < 0.0f)
  {
    mBallVel.x *= -1.0f;
  }

  SDL_SetWindowPosition(mWindows[1], mBall.screenPos.x, mBall.screenPos.y);
  SDL_SetWindowPosition(mWindows[2], mPaddle.screenPos.x, mPaddle.screenPos.y);
}

void Game::GenerateOutput()
{
  SDL_SetRenderDrawColor(mRenderers[0], 0, 0, 0, 255);
  SDL_RenderClear(mRenderers[0]);

  SDL_RenderPresent(mRenderers[0]);

  // ボールの描画
  SDL_SetRenderDrawColor(mRenderers[1], 0, 0, 0, 255);
  SDL_RenderClear(mRenderers[1]);
  SDL_SetRenderDrawColor(mRenderers[1], 255, 255, 255, 255);
  SDL_Rect ball{
      static_cast<int>(mBall.pos.x - thickness / 2.0f),
      static_cast<int>(mBall.pos.y - thickness / 2.0f),
      thickness,
      thickness};
  SDL_RenderFillRect(mRenderers[1], &ball);
  SDL_RenderPresent(mRenderers[1]);

  // パドルの描画
  SDL_SetRenderDrawColor(mRenderers[2], 0, 0, 0, 255);
  SDL_RenderClear(mRenderers[2]);
  SDL_SetRenderDrawColor(mRenderers[2], 255, 255, 255, 255);
  SDL_Rect paddle{
      static_cast<int>(mPaddle.pos.x - thickness / 2.0f),
      static_cast<int>(mPaddle.pos.y - paddleWidth / 2.0f),
      thickness,
      static_cast<int>(paddleWidth)};
  SDL_RenderFillRect(mRenderers[2], &paddle);

  if (mBall.GetWorldPos().x < mPaddle.screenPos.x + mPaddle.size.x - thickness / 2.0f &&
      mBall.GetWorldPos().x >= mPaddle.screenPos.x + thickness / 2.0f &&
      mBall.GetWorldPos().y < mPaddle.screenPos.y + mPaddle.size.y - thickness / 2.0f &&
      mBall.GetWorldPos().y >= mPaddle.screenPos.y + thickness / 2.0f)
  {
    SDL_Rect ball{
        static_cast<int>(mBall.GetWorldPos().x - mPaddle.screenPos.x - thickness / 2.0f),
        static_cast<int>(mBall.GetWorldPos().y - mPaddle.screenPos.y - thickness / 2.0f),
        thickness,
        thickness};
    SDL_RenderFillRect(mRenderers[2], &ball);
  }
  SDL_RenderPresent(mRenderers[2]);
}