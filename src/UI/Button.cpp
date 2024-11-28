#include "Button.h"

Button::Button(const std::string &text, Vector2 pos, Vector2 size, SDL_Color normalColor, SDL_Color hoverColor, SDL_Color textColor)
    : mText(text), mPosition(pos), mSize(size), mNormalColor(normalColor),
      mHoverColor(hoverColor), mTextColor(textColor), mIsHovered(false), mOnClick(nullptr)
{
}

void Button::HandleClick(const Vector2 &mousePos)
{
  // ボタンの領域を定義
  SDL_Rect buttonRect = {
      static_cast<int>(mPosition.x),
      static_cast<int>(mPosition.y),
      static_cast<int>(mSize.x),
      static_cast<int>(mSize.y)};

  // マウス位置をSDL_Pointとして定義
  SDL_Point point = {
      static_cast<int>(mousePos.x),
      static_cast<int>(mousePos.y)};

  // ボタンの領域内にマウスがあるかチェック
  if (SDL_PointInRect(&point, &buttonRect))
  {
    if (mOnClick)
    {
      mOnClick();
    }
  }
}

void Button::Update(const Vector2 &mousePos)
{
  SDL_Rect rect = {
      static_cast<int>(mPosition.x),
      static_cast<int>(mPosition.y),
      static_cast<int>(mSize.x),
      static_cast<int>(mSize.y)};
  SDL_Point point = mousePos.ToSDLPoint();
  mIsHovered = SDL_PointInRect(&point, &rect);

  // デバッグ出力
  if (mIsHovered)
  {
    SDL_Log("Button Hovered - Mouse: (%f, %f), Button: (%f, %f, %f, %f)",
            mousePos.x, mousePos.y,
            mPosition.x, mPosition.y,
            mPosition.x + mSize.x, mPosition.y + mSize.y);
  }
}

void Button::Draw(SDL_Renderer *renderer, TextRenderer *textRenderer)
{
  SDL_Color currentColor = mIsHovered ? mHoverColor : mNormalColor;
  SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
  SDL_Rect rect = {static_cast<int>(mPosition.x), static_cast<int>(mPosition.y),
                   static_cast<int>(mSize.x), static_cast<int>(mSize.y)};
  SDL_RenderFillRect(renderer, &rect);

  // テキストの描画（中央配置）
  int textWidth, textHeight;
  textRenderer->GetTextSize(mText, textWidth, textHeight);
  float textX = mPosition.x + (mSize.x - textWidth) / 2.0f;
  float textY = mPosition.y + (mSize.y - textHeight) / 2.0f;
  textRenderer->RenderText(mText, static_cast<int>(textX), static_cast<int>(textY), mTextColor, renderer);
}
