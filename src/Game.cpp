#include "Game.h"

const int thickness = 15;
const float paddleWidth = 100.0f;
const int masterWindowHeight = 200;
const int windowWidth = 200;
const int windowHeight = 200;
const float paddleSpeed = 600.0f;

Game::Game() : mIsRunning(true), mTicksCount(0), mPaddleDir(0.0f), mCurrentState(GameState::Start), mScore(0)
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
  mBall.pos = {windowWidth / 2.0f + mBall.screenPos.x, windowHeight / 2.0f + mBall.screenPos.y};
  mBallVel = {-200.0f, 235.0f};

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
  mPaddle.pos = {windowWidth / 2.0f + mPaddle.screenPos.x, windowHeight / 2.0f + mPaddle.screenPos.y};

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

  if (paddleTop < masterWindowHeight)
  {
    mPaddle.pos.y = masterWindowHeight + paddleWidth / 2.0f;
  }
  else if (paddleBottom > mScreen.size.y)
  {
    mPaddle.pos.y = mScreen.size.y - paddleWidth / 2.0f;
  }

  // パドルウィンドウの位置制限
  mPaddle.screenPos.y = mPaddle.pos.y - mPaddle.size.y / 2.0f;

  // ウィンドウの上端がmasterWindowの下端より上にならないようにする
  if (mPaddle.screenPos.y < masterWindowHeight)
  {
    mPaddle.screenPos.y = masterWindowHeight;
  }
  // ウィンドウの下端が画面の下端を超えないようにする
  else if (mPaddle.screenPos.y + mPaddle.size.y > mScreen.size.y)
  {
    mPaddle.screenPos.y = mScreen.size.y - mPaddle.size.y;
  }

  // ウィンドウ位置を整数値に丸める（ピクセル単位で正確に配置するため）
  mPaddle.screenPos.y = std::round(mPaddle.screenPos.y);

  // ウィンドウ位置更新
  SDL_SetWindowPosition(mWindows[2], static_cast<int>(mPaddle.screenPos.x), static_cast<int>(mPaddle.screenPos.y));

  mBall.pos.x += mBallVel.x * deltaTime;
  mBall.pos.y += mBallVel.y * deltaTime;
  if (mBall.GetLocalPos().x < thickness / 2.0f)
  {
    // mBall.pos.x = thickness / 2.0f;
    mBall.screenPos.x += mBallVel.x * deltaTime;
  }
  if (mBall.GetLocalPos().x > mBall.size.x - thickness / 2.0f)
  {
    // mBall.pos.x = mBall.size.x - thickness / 2.0f;
    mBall.screenPos.x += mBallVel.x * deltaTime;
  }

  if (mBall.GetLocalPos().y < thickness / 2.0f)
  {
    // mBall.pos.y = thickness / 2.0f;
    mBall.screenPos.y += mBallVel.y * deltaTime;
  }
  if (mBall.GetLocalPos().y > mBall.size.y - thickness / 2.0f)
  {
    // mBall.pos.y = mBall.size.y - thickness / 2.0f;
    mBall.screenPos.y += mBallVel.y * deltaTime;
  }

  // ボールとパドルの衝突判定
  float diffX = mPaddle.pos.x - mBall.pos.x;
  float diffY = mPaddle.pos.y - mBall.pos.y;

  if (std::abs(diffY) <= paddleWidth / 2.0f && std::abs(diffX) <= thickness / 2.0f)
  {
    mBallVel.x *= -1.0f;
  }

  // ボールの壁反射
  if ((mBall.pos.y - thickness / 2.0f <= masterWindowHeight && mBallVel.y < 0.0f) || (mBall.pos.y + thickness / 2.0f >= mScreen.size.y && mBallVel.y > 0.0f))
  {
    mBallVel.y *= -1.0f;
  }
  if (mBall.pos.x >= mScreen.size.x - thickness / 2.0f && mBallVel.x > 0.0f)
  {
    mBallVel.x *= -1.0f;
  }
  if (mBall.pos.x <= thickness / 2.0f && mBallVel.x < 0.0f)
  {
    mBallVel.x *= -1.0f;
  }

  // ウィンドウ位置更新
  SDL_SetWindowPosition(mWindows[1], mBall.screenPos.x, mBall.screenPos.y);
}

void Game::GenerateOutput()
{
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
