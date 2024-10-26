#include "TextRenderer.h"

TextRenderer::TextRenderer(const std::string &fontPath, int fontSize)
{
  mFontSize = fontSize;
  mFont = TTF_OpenFont(fontPath.c_str(), fontSize);
  if (!mFont)
  {
    SDL_Log("フォントのロードに失敗しました: %s", TTF_GetError());
  }
}

TextRenderer::~TextRenderer()
{
  TTF_CloseFont(mFont);
}

void TextRenderer::RenderText(
    const std::string &text,
    int x,
    int y,
    SDL_Color color,
    SDL_Renderer *renderer)
{
  SDL_Surface *surface = TTF_RenderText_Solid(mFont, text.c_str(), color);
  if (surface == nullptr)
  {
    SDL_Log("テキストのレンダリングに失敗しました: %s", TTF_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr)
  {
    SDL_Log("テクスチャの作成に失敗しました: %s", SDL_GetError());
    SDL_FreeSurface(surface);
    return;
  }

  SDL_Rect dstRect = {x, y, surface->w, surface->h};
  SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}
