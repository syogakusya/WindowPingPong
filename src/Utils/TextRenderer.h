#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
class TextRenderer
{
public:
  TextRenderer(const std::string &fontPath, int fontSize);
  ~TextRenderer();
  void RenderText(
      const std::string &text,
      int x,
      int y,
      SDL_Color color,
      SDL_Renderer *renderer);

  enum class RenderType
  {
    Solid,
    Shaded,
    Blended,
    LCD
  };

  void SetFontSize(int fontSize);

  void GetTextSize(const std::string &text, int &width, int &height) const;

private:
  TTF_Font *mFont;
  int mFontSize;
  std::string mFontPath;
  std::string GetFontPath(const std::string &fontPath);
};
