#pragma once
#include "Vector2.h"
#include <SDL.h>
#include <functional>

class GameObject
{
public:
  class Proxy
  {
  public:
    Proxy(Vector2 &ref, std::function<void()> updateFunc)
        : ref(ref), updateFunc(updateFunc), x(ref.x), y(ref.y) {}

    float &x;
    float &y;

    operator Vector2() const { return ref; }

    Proxy &operator=(const Vector2 &other)
    {
      ref = other;
      updateFunc();
      return *this;
    }

    Proxy &operator+=(const Vector2 &other)
    {
      ref += other;
      updateFunc();
      return *this;
    }

    Proxy &operator-=(const Vector2 &other)
    {
      ref -= other;
      updateFunc();
      return *this;
    }

    Proxy &operator*=(float scalar)
    {
      ref *= scalar;
      updateFunc();
      return *this;
    }

    Proxy &operator/=(float scalar)
    {
      ref /= scalar;
      updateFunc();
      return *this;
    }

  private:
    Vector2 &ref;
    std::function<void()> updateFunc;
  };

  GameObject(const char *windowName, int x, int y, int width, int height, Uint32 windowFlags);
  virtual ~GameObject();

  virtual void Update(float deltaTime) = 0;
  virtual void Draw(SDL_Renderer *renderer) = 0;

  void UpdateWindowPosition();

  SDL_Window *GetWindow() const { return mWindow; }
  SDL_Renderer *GetRenderer() const { return mRenderer; }

  Proxy worldPos;
  Proxy localPos;
  Proxy velocity;

protected:
  Vector2 mWorldPos;
  Vector2 mLocalPos;
  Vector2 mVelocity;
  Vector2 mSize;
  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;

  void UpdateLocalPos();
};
