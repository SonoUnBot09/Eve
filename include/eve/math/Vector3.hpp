#pragma once

#include <cmath>
#include <algorithm>

namespace Eve::Math
{
    struct Vector3
    {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {};
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
        Vector3(float x) : x(x), y(x), z(x) {};

        Vector3 operator+ (Vector3 other) const
        {
            return Vector3(x + other.x, y + other.y, z + other.z);
        }
        Vector3 operator- (Vector3 other) const
        {
            return Vector3(x - other.x, y - other.y, z - other.z);
        }
        Vector3 operator* (float scalar) const
        {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }
        friend Vector3 operator* (float scalar, Vector3 a)
        {
            return Vector3(scalar * a.x, scalar * a.y, scalar * a.z);
        }
        Vector3 operator/ (float scalar) const
        {
            return Vector3(x / scalar, y / scalar, z / scalar);
        }
        friend Vector3 operator/ (float scalar, Vector3 a)
        {
            return Vector3(scalar / a.x, scalar / a.y, scalar / a.z);
        }
        Vector3 operator- () const
        {
            return Vector3(-x, -y, -z);
        }

        bool operator==(Vector3 a) const
        {
            return x == a.x && y == a.y && z == a.z;
        }

        inline void Normalize()
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
                z = z / magnitude;
            }
        };

        inline Vector3 Normalized() const
        {
            float magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector3(x / magnitude, y / magnitude, z / magnitude);
            }

            return Vector3(0, 0, 0);
        };

        inline float Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        inline float SqrMagnitude() const
        {
            return x * x + y * y + z * z;
        }

        inline static float Dot(Vector3 a, Vector3 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z; 
        };

        inline static Vector3 Cross(Vector3 a, Vector3 b)
        {
            return Vector3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }

        inline static Vector3 Lerp(Vector3 a, Vector3 b, float t)
        {
            t = std::clamp(t, 0.0f, 1.0f);

            return Vector3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        };

        inline static Vector3 LerpUnclamped(Vector3 a, Vector3 b, float t)
        {
            return Vector3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        };
    };
}