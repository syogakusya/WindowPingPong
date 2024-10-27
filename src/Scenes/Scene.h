#pragma once
#include <SDL.h>

class Scene
{
public:
  virtual ~Scene() {}
  virtual void Initialize() = 0;
  virtual void HandleInput(const Uint8 *keyBoardState) = 0;
  virtual void Update(float deltaTime) = 0;
  virtual void Render() = 0;
  virtual void Shutdown() = 0;
};
