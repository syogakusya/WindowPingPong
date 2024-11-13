#include "GameObject.h"

Vector2 GameObject::mScreenSize = Vector2(0, 0);

GameObject::GameObject(
    const char *windowName,
    Vector2 pos, Vector2 size, Uint32 windowFlags)
    : mWorldPos{pos.x, pos.y},
      mLocalPos{size.x / 2, size.y / 2},
      mVelocity{0.0f, 0.0f},
      mWindowSize{size.x, size.y},
      mWindowPos{pos.x - size.x / 2, pos.y - size.y / 2},
      mIsShaking(false),
      mShakeTime(0.0f),
      mShakeIntensity(0.0f),
      mShakeOffset(0.0f, 0.0f),
      mOriginalWindowPos(mWindowPos),
      mOriginalWorldPos(mWorldPos),
      mRandomEngine(std::random_device{}()),
      mRandomDistribution(-1.0f, 1.0f)
{
#ifdef __APPLE__
  windowFlags |= SDL_WINDOW_METAL | SDL_WINDOW_ALLOW_HIGHDPI;
#endif

  // ウィンドウの作成
  mWindow = SDL_CreateWindow(
      windowName,
      static_cast<int>(mWindowPos.x),
      static_cast<int>(mWindowPos.y),
      static_cast<int>(mWindowSize.x),
      static_cast<int>(mWindowSize.y),
      windowFlags);

  if (!mWindow)
  {
    SDL_Log("ウィンドウの作成に失敗しました: %s", SDL_GetError());
    return;
  }

  // HiDPIスケーリングを取得
  int drawableWidth, drawableHeight;
  SDL_GL_GetDrawableSize(mWindow, &drawableWidth, &drawableHeight);
  float scaleX = drawableWidth / mWindowSize.x;
  float scaleY = drawableHeight / mWindowSize.y;

  Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
#ifdef __APPLE__
  rendererFlags |= SDL_RENDERER_TARGETTEXTURE;
#endif

  mRenderer = SDL_CreateRenderer(mWindow, -1, rendererFlags);

  if (!mRenderer)
  {
    SDL_Log("レンダラーの作成に失敗しました: %s", SDL_GetError());
    return;
  }

  // レンダラーのスケールを設定
  SDL_RenderSetScale(mRenderer, scaleX, scaleY);
}

GameObject::~GameObject()
{
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
}

// ローカル座標を再計算し、ウィンドウの位置を再設定する
void GameObject::UpdateWindowPosition()
{
  UpdateLocalPos();
  SDL_SetWindowPosition(mWindow,
                        static_cast<int>(mWindowPos.x),
                        static_cast<int>(mWindowPos.y));
}

void GameObject::UpdateLocalPos()
{
  mLocalPos = mWorldPos - mWindowPos;
}

void GameObject::RenderPresent(SDL_Renderer *renderer)
{
  SDL_RenderPresent(renderer);
}

void GameObject::ShowWindow()
{
  SDL_ShowWindow(mWindow);
}

void GameObject::HideWindow()
{
  SDL_HideWindow(mWindow);
}

bool GameObject::IsWindowVisible()
{
  return SDL_GetWindowFlags(mWindow) & SDL_WINDOW_SHOWN;
}

void GameObject::UpdateShakeEffect(float deltaTime)
{
  if (mIsShaking)
  {
    mShakeTime -= deltaTime;
    if (mShakeTime <= 0.0f)
    {
      mIsShaking = false;
      mShakeOffset = Vector2(0.0f, 0.0f);
      mWindowPos = mOriginalWindowPos;
      mWorldPos = mOriginalWorldPos;
    }
    else
    {
      float randX = mRandomDistribution(mRandomEngine);
      float randY = mRandomDistribution(mRandomEngine);
      mShakeOffset = Vector2(randX, randY) * mShakeIntensity;
      mWindowPos = mOriginalWindowPos + mShakeOffset;
      mWorldPos = mOriginalWorldPos + mShakeOffset;
    }
  }
}

void GameObject::StartShake(float duration, float intensity)
{
  mIsShaking = true;
  mShakeTime = duration;
  mShakeIntensity = intensity;
  mOriginalWindowPos = mWindowPos;
  mOriginalWorldPos = mWorldPos;
}

bool GameObject::CheckWindowCollision(SDL_Rect *other) const
{
  SDL_Rect rect1 = GetWindowRect();
  return SDL_HasIntersection(&rect1, other);
}

SDL_Rect GameObject::GetWindowRect() const
{
  SDL_Rect rect;
  rect.x = static_cast<int>(mWindowPos.x);
  rect.y = static_cast<int>(mWindowPos.y);
  rect.w = static_cast<int>(mWindowSize.x);
  rect.h = static_cast<int>(mWindowSize.y);
  return rect;
}
