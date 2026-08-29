#pragma once

#include <cmath>

namespace Eve::Math
{
    struct Vector2Int
    {
        int32_t x, y;

        Vector2Int() : x(0), y(0) {};
        Vector2Int(int32_t x, int32_t y) : x(x), y(y) {};
        Vector2Int(int32_t x) : x(x), y(x) {};

        Vector2Int operator+ (Vector2Int other) const
        {
            return Vector2Int(x + other.x, y + other.y);
        }
        Vector2Int operator- (Vector2Int other) const
        {
            return Vector2Int(x - other.x, y - other.y);
        }
        Vector2Int operator* (int32_t scalar) const
        {
            return Vector2Int(x * scalar, y * scalar);
        }
        friend Vector2Int operator* (int32_t scalar, Vector2Int a)
        {
            return Vector2Int(scalar * a.x, scalar * a.y);
        }
        Vector2Int operator/ (int32_t scalar) const
        {
            return Vector2Int(x / scalar, y / scalar);
        }
        friend Vector2Int operator/ (int32_t scalar, Vector2Int a)
        {
            return Vector2Int(scalar / a.x, scalar / a.y);
        }
        Vector2Int operator- () const
        {
            return Vector2Int(-x, -y);
        }

        bool operator==(Vector2Int a) const
        {
            return x == a.x && y == a.y;
        }

        inline void Normalize()
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
            }
        };

        inline Vector2Int Normalized() const
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector2Int(x / magnitude, y / magnitude);
            }

            return Vector2Int(0, 0);
        };

        inline int32_t Magnitude() const
        {
            return std::sqrt(x * x + y * y);
        }

        inline int32_t SqrMagnitude() const
        {
            return x * x + y * y;
        }

        inline static int32_t Dot(Vector2Int a, Vector2Int b)
        {
            return a.x * b.x + a.y * b.y; 
        };
    };
}