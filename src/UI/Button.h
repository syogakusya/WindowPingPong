#pragma once
#include <SDL.h>
#include <string>
#include <functional>
#include "../Utils/Vector2.h"
#include "../Utils/TextRenderer.h"

class Button
{
public:
  Button(const std::string &text, Vector2 position, Vector2 size,
         SDL_Color normalColor, SDL_Color hoverColor);

  void Update(const Vector2 &mousePos);
  void Draw(SDL_Renderer *renderer, TextRenderer *textRenderer);
  void SetOnClick(std::function<void()> callback) { mOnClick = callback; }
  bool HandleClick(const Vector2 &mousePos);

private:
  std::string mText;
  Vector2 mPosition;
  Vector2 mSize;
  SDL_Color mNormalColor;
  SDL_Color mHoverColor;
  SDL_Color mCurrentColor;
  bool mIsHovered;
  std::function<void()> mOnClick;

  bool IsInside(const Vector2 &point) const;
};