#pragma once
#include <SDL.h>

class Scene
{
public:
  virtual ~Scene() {}
  virtual void HandleInput(const Uint8 *keyState) = 0;
  virtual void Update(float deltaTime) = 0;
  virtual void Render() = 0;
};
