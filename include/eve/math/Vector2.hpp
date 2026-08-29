#pragma once

#include <cmath>
#include <algorithm>

namespace Eve::Math
{
    struct Vector2
    {
        float x, y;

        Vector2() : x(0), y(0) {};
        Vector2(float x, float y) : x(x), y(y) {};
        Vector2(float x) : x(x), y(x) {};

        Vector2 operator+ (Vector2 other) const
        {
            return Vector2(x + other.x, y + other.y);
        }
        Vector2 operator- (Vector2 other) const
        {
            return Vector2(x - other.x, y - other.y);
        }
        Vector2 operator* (float scalar) const
        {
            return Vector2(x * scalar, y * scalar);
        }
        friend Vector2 operator* (float scalar, Vector2 a)
        {
            return Vector2(scalar * a.x, scalar * a.y);
        }
        Vector2 operator/ (float scalar) const
        {
            return Vector2(x / scalar, y / scalar);
        }
        friend Vector2 operator/ (float scalar, Vector2 a)
        {
            return Vector2(scalar / a.x, scalar / a.y);
        }
        Vector2 operator- () const
        {
            return Vector2(-x, -y);
        }

        bool operator==(Vector2 a) const
        {
            return x == a.x && y == a.y;
        }

        inline void Normalize()
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
            }
        };

        inline Vector2 Normalized() const
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector2(x / magnitude, y / magnitude);
            }

            return Vector2(0, 0);
        };

        inline float Magnitude() const
        {
            return std::sqrt(x * x + y * y);
        }

        inline float SqrMagnitude() const
        {
            return x * x + y * y;
        }

        inline static float Dot(Vector2 a, Vector2 b)
        {
            return a.x * b.x + a.y * b.y; 
        };

        inline static Vector2 Lerp(Vector2 a, Vector2 b, float t)
        {
            t = std::clamp(t, 0.0f, 1.0f);

            return Vector2(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t
            );
        };

        inline static Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t)
        {
            return Vector2(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t
            );
        };
    };
}