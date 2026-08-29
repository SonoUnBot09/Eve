#pragma once

#include <cmath>
#include <algorithm>

namespace Eve::Math
{
    struct Vector4
    {
        float x, y, z, w;

        Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
        Vector4(float x) : x(x), y(x), z(x), w(x) {};

        Vector4 operator+ (Vector4 other) const
        {
            return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
        }
        Vector4 operator- (Vector4 other) const
        {
            return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
        }
        Vector4 operator* (float scalar) const
        {
            return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
        }
        friend Vector4 operator* (float scalar, Vector4 a)
        {
            return Vector4(scalar * a.x, scalar * a.y, scalar * a.z, scalar * a.w);
        }
        Vector4 operator/ (float scalar) const
        {
            return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
        }
        friend Vector4 operator/ (float scalar, Vector4 a)
        {
            return Vector4(scalar / a.x, scalar / a.y, scalar / a.z, scalar / a.w);
        }
        Vector4 operator- () const
        {
            return Vector4(-x, -y, -z, -w);
        }
        bool operator==(Vector4 a) const
        {
            return x == a.x && y == a.y && z == a.z && w == a.w;
        }

        inline void Normalize()
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
                z = z / magnitude;
                w = w / magnitude;
            }
        };

        inline Vector4 Normalized() const
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector4(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
            }

            return Vector4(0, 0, 0, 0);
        };

        inline float Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        inline float SqrMagnitude() const
        {
            return x * x + y * y + z * z + w * w;
        }

        inline static float Dot(Vector4 a, Vector4 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; 
        };

        inline static Vector4 Lerp(Vector4 a, Vector4 b, float t)
        {
            t = std::clamp(t, 0.0f, 1.0f);

            return Vector4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            );
        };

        inline static Vector4 LerpUnclamped(Vector4 a, Vector4 b, float t)
        {
            return Vector4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            );
        };
    };
}