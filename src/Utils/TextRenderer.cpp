#include "TextRenderer.h"
#include <iostream>
// フォントパスの取得
std::string TextRenderer::GetFontPath(const std::string &fontPath)
{
  return "../Resources/fonts/" + fontPath;
}

TextRenderer::TextRenderer(const std::string &fontPath, int fontSize)
    : mFontSize(fontSize)
{
  if (TTF_Init() == -1)
  {
    SDL_Log("TTF_Init: %s\n", TTF_GetError());
    return;
  }

  mFont = TTF_OpenFont(GetFontPath(fontPath).c_str(), fontSize);
  if (!mFont)
  {
    SDL_Log("TTF_OpenFont: %s\n", TTF_GetError());
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
    // SDL_Log("テキストのレンダリングに失敗しました: %s", TTF_GetError());
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
