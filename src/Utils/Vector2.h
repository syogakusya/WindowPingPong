#pragma once
#include <cmath>

struct Vector2
{
  float x;
  float y;

  Vector2() : x(0), y(0) {}

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
  Vector2 operator*(float scalar) const
  {
    return Vector2{x * scalar, y * scalar};
  }
  Vector2 &operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  };
  Vector2 operator/(float scalar) const
  {
    return Vector2{x / scalar, y / scalar};
  }
  Vector2 &operator/=(float scalar)
  {
    x /= scalar;
    y /= scalar;
    return *this;
  };

  float Length() const
  {
    return std::sqrt(x * x + y * y);
  }

  float LengthSquared() const
  {
    return x * x + y * y;
  }

  void Normalize()
  {
    float length = Length();
    if (length > 0)
    {
      x /= length;
      y /= length;
    }
  }

  Vector2 Normalized() const
  {
    Vector2 result = *this;
    result.Normalize();
    return result;
  }

  float Dot(const Vector2 &other) const
  {
    return x * other.x + y * other.y;
  }

  static float Distance(const Vector2 &a, const Vector2 &b)
  {
    return (b - a).Length();
  }
};
