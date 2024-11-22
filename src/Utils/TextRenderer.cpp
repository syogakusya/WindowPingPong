#include "TextRenderer.h"
#include <iostream>
// フォントパスの取得
std::string TextRenderer::GetFontPath(const std::string &fontPath)
{
  return "../../fonts/" + fontPath;
}

TextRenderer::TextRenderer(const std::string &fontPath, int fontSize)
    : mFontSize(fontSize),
      mFontPath(fontPath)
{
  if (TTF_Init() == -1)
  {
    SDL_Log("TTF_Init: %s\n", TTF_GetError());
    return;
  }

  mFont = TTF_OpenFont(GetFontPath(mFontPath).c_str(), mFontSize);
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

void TextRenderer::SetFontSize(int fontSize)
{
  if (mFontSize == fontSize)
    return;

  mFontSize = fontSize;

  // 既存のフォントを解放
  if (mFont)
  {
    TTF_CloseFont(mFont);
  }

  // 新しいサイズでフォントを再読み込み
  mFont = TTF_OpenFont(GetFontPath(mFontPath).c_str(), fontSize);
  if (!mFont)
  {
    SDL_Log("フォントの再読み込みに失敗しました: %s\n", TTF_GetError());
  }
}

void TextRenderer::GetTextSize(const std::string &text, int &width, int &height) const
{
  if (TTF_SizeText(mFont, text.c_str(), &width, &height) != 0)
  {
    SDL_Log("TTF_SizeText: %s\n", TTF_GetError());
    width = height = 0;
  }
}
