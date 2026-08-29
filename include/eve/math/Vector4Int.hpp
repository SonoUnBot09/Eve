#pragma once

#include <cmath>

namespace Eve::Math
{
    struct Vector4Int
    {
        int32_t x, y, z, w;

        Vector4Int() : x(0), y(0), z(0), w(0) {};
        Vector4Int(int32_t x, int32_t y, int32_t z, int32_t w) : x(x), y(y), z(z), w(w) {};
        Vector4Int(int32_t x) : x(x), y(x), z(x), w(x) {};

        Vector4Int operator+ (Vector4Int other) const
        {
            return Vector4Int(x + other.x, y + other.y, z + other.z, w + other.w);
        }
        Vector4Int operator- (Vector4Int other) const
        {
            return Vector4Int(x - other.x, y - other.y, z - other.z, w - other.w);
        }
        Vector4Int operator* (int32_t scalar) const
        {
            return Vector4Int(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        friend Vector4Int operator* (int32_t scalar, Vector4Int a)
        {
            return Vector4Int(scalar * a.x, scalar * a.y, scalar * a.z, scalar * a.w);
        }
        Vector4Int operator/ (int32_t scalar) const
        {
            return Vector4Int(x / scalar, y / scalar, z / scalar, w / scalar);
        }
        friend Vector4Int operator/ (int32_t scalar, Vector4Int a)
        {
            return Vector4Int(scalar / a.x, scalar / a.y, scalar / a.z, scalar / a.w);
        }
        Vector4Int operator- () const
        {
            return Vector4Int(-x, -y, -z, -w);
        }
        bool operator==(Vector4Int a) const
        {
            return x == a.x && y == a.y && z == a.z && w == a.w;
        }

        inline void Normalize()
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
                z = z / magnitude;
                w = w / magnitude;
            }
        };

        inline Vector4Int Normalized() const
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector4Int(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
            }

            return Vector4Int(0, 0, 0, 0);
        };

        inline int32_t Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        inline int32_t SqrMagnitude() const
        {
            return x * x + y * y + z * z + w * w;
        }

        inline static int32_t Dot(Vector4Int a, Vector4Int b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; 
        };
    };
}