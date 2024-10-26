#pragma once
#include "Scene.h"
#include "../Utils/TextRenderer.h"

class StartScene : public Scene
{
public:
  StartScene(SDL_Renderer *renderer, TextRenderer *textRenderer);
  void HandleInput(const Uint8 *keyState) override;
  void Update(float deltaTime) override;
  void Render() override;

private:
  SDL_Renderer *mRenderer;
  TextRenderer *mTextRenderer;
};
