#include "ObstacleWindow.h"
#include <iostream>

ObstacleWindow::ObstacleWindow(
    const char *windowName,
    Vector2 pos,
    Vector2 size,
    Type type,
    float duration,
    int closeCount)
    : GameObject(windowName, pos, size,
                 (type == Type::Transparent ? SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP : 0)),
      mType(type),
      mLifeTime(duration),
      mDuration(duration),
      mIsActive(true),
      mCloseCount(closeCount),
      mCurrentCloseCount(0),
      mCurrentTexture(nullptr)
{
  if (mType == Type::Popup)
  {
    LoadAdImages();
    // ランダムに広告タイプを選択
    mAdType = static_cast<AdType>(rand() % 3);
    mCurrentTexture = mAdTextures[static_cast<int>(mAdType)];
  }
}

ObstacleWindow::~ObstacleWindow()
{
  for (auto texture : mAdTextures)
  {
    if (texture)
    {
      SDL_DestroyTexture(texture);
    }
  }
}

void ObstacleWindow::LoadAdImages()
{
  const std::vector<std::string> paths = {
      "assets/images/ad1.jpg",
      "assets/images/ad2.jpg",
      "assets/images/ad3.jpg"};

  for (const auto &path : paths)
  {
    SDL_Texture *texture = LoadTexture(path);
    if (texture)
    {
      mAdTextures.push_back(texture);
    }
  }
}

SDL_Texture *ObstacleWindow::LoadTexture(const std::string &path)
{
  SDL_Log("画像を読み込み中: %s", path.c_str());

  SDL_Surface *surface = IMG_Load(path.c_str());
  if (!surface)
  {
    SDL_Log("画像の読み込みに失敗: %s", IMG_GetError());
    return nullptr;
  }

  SDL_Log("画像の読み込み成功: %dx%d", surface->w, surface->h);

  SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
  SDL_FreeSurface(surface);

  if (!texture)
  {
    SDL_Log("テクスチャの作成に失敗: %s", SDL_GetError());
    return nullptr;
  }

  SDL_Log("テクスチャの作成成功");
  return texture;
}

void ObstacleWindow::Update(float deltaTime)
{
  if (!mIsActive)
    return;

  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (event.type == SDL_WINDOWEVENT &&
        event.window.windowID == SDL_GetWindowID(mWindow))
    {
      if (event.window.event == SDL_WINDOWEVENT_CLOSE)
      {
        HandleClick();
      }
    }
  }

  if (mType == Type::Transparent)
  {
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f)
    {
      mIsActive = false;
    }
  }
  UpdateShakeEffect(deltaTime);
  UpdateWindowPosition();
}

void ObstacleWindow::Draw(SDL_Renderer *renderer)
{
  // 背景をクリア
  if (mType == Type::Transparent)
  {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
  }
  else
  {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  }
  SDL_RenderClear(renderer);

  // ポップアップタイプの場合、画像を描画
  if (mType == Type::Popup && mCurrentTexture)
  {
    // テクスチャのサイズを取得
    int texW, texH;
    SDL_QueryTexture(mCurrentTexture, NULL, NULL, &texW, &texH);

    // ウィンドウサイズに合わせてテクスチャを描画
    SDL_Rect dstRect = {
        0,                               // x
        0,                               // y
        static_cast<int>(mWindowSize.x), // width
        static_cast<int>(mWindowSize.y)  // height
    };

    // テクスチャを描画
    SDL_RenderCopy(renderer, mCurrentTexture, NULL, &dstRect);
  }
}

void ObstacleWindow::HandleClick()
{
  std::cout << "HandleClick" << std::endl;
  if (mType == Type::Popup)
  {
    mCurrentCloseCount++;
    if (mCurrentCloseCount >= mCloseCount)
    {
      mIsActive = false;
    }
  }
}