#include "Button.h"

Button::Button(const std::string &text, Vector2 position, Vector2 size,
               SDL_Color normalColor, SDL_Color hoverColor)
    : mText(text), mPosition(position), mSize(size), mNormalColor(normalColor), mHoverColor(hoverColor), mCurrentColor(normalColor), mIsHovered(false)
{
}

void Button::Update(const Vector2 &mousePos)
{
  // ホバーを検知
  mIsHovered = IsInside(mousePos);
  mCurrentColor = mIsHovered ? mHoverColor : mNormalColor;
}

void Button::Draw(SDL_Renderer *renderer, TextRenderer *textRenderer)
{
  SDL_Rect buttonRect = {
      static_cast<int>(mPosition.x),
      static_cast<int>(mPosition.y),
      static_cast<int>(mSize.x),
      static_cast<int>(mSize.y)};

  // ボタンの背景を描画
  SDL_SetRenderDrawColor(
      renderer,
      mCurrentColor.r, mCurrentColor.g, mCurrentColor.b, mCurrentColor.a);
  SDL_RenderFillRect(renderer, &buttonRect);

  // テキストを中央揃えで描画
  SDL_Color textColor = {255, 255, 255, 255};
  int textX = mPosition.x + (mSize.x - mText.length() * 12) / 2; // 12は文字の平均幅
  int textY = mPosition.y + (mSize.y - 24) / 2;                  // 24はフォントサイズ
  textRenderer->RenderText(mText, textX, textY, textColor, renderer);
}

bool Button::HandleClick(const Vector2 &mousePos)
{
  if (IsInside(mousePos) && mOnClick)
  {
    mOnClick();
    return true;
  }
  return false;
}

bool Button::IsInside(const Vector2 &point) const
{
  return point.x >= mPosition.x && point.x <= mPosition.x + mSize.x &&
         point.y >= mPosition.y && point.y <= mPosition.y + mSize.y;
}