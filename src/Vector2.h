#pragma once

struct Vector2
{
  float x;
  float y;

  Vector2(float x, float y)
  {
    this->x = x;
    this->y = y;
  }

  Vector2 operator+(const Vector2 &other) const
  {
    return Vector2{x + other.x, y + other.y};
  }
  Vector2 &operator+=(const Vector2 &other)
  {
    x += other.x;
    y += other.y;
    return *this;
  };
  Vector2 operator-(const Vector2 &other) const
  {
    return Vector2{x - other.x, y - other.y};
  }
  Vector2 &operator-=(const Vector2 &other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  };
  Vector2 &operator*(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  };
  Vector2 &operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  };
  Vector2 operator/(float scalar)
  {
    return Vector2{x / scalar, y / scalar};
  }
  Vector2 &operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    return *this;
  };
};
