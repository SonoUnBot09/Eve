#pragma once

#include <cmath>

namespace Eve::Math
{
    struct Vector3Int
    {
        int32_t x, y, z;

        Vector3Int() : x(0), y(0), z(0) {};
        Vector3Int(int32_t x, int32_t y, int32_t z) : x(x), y(y), z(z) {};
        Vector3Int(int32_t x) : x(x), y(x), z(x) {};

        Vector3Int operator+ (Vector3Int other) const
        {
            return Vector3Int(x + other.x, y + other.y, z + other.z);
        }
        Vector3Int operator- (Vector3Int other) const
        {
            return Vector3Int(x - other.x, y - other.y, z - other.z);
        }
        Vector3Int operator* (int32_t scalar) const
        {
            return Vector3Int(x * scalar, y * scalar, z * scalar);
        }
        friend Vector3Int operator* (int32_t scalar, Vector3Int a)
        {
            return Vector3Int(scalar * a.x, scalar * a.y, scalar * a.z);
        }
        Vector3Int operator/ (int32_t scalar) const
        {
            return Vector3Int(x / scalar, y / scalar, z / scalar);
        }
        friend Vector3Int operator/ (int32_t scalar, Vector3Int a)
        {
            return Vector3Int(scalar / a.x, scalar / a.y, scalar / a.z);
        }
        Vector3Int operator- () const
        {
            return Vector3Int(-x, -y, -z);
        }

        bool operator==(Vector3Int a) const
        {
            return x == a.x && y == a.y && z == a.z;
        }

        inline void Normalize()
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                x = x / magnitude;
                y = y / magnitude;
                z = z / magnitude;
            }
        };

        inline Vector3Int Normalized() const
        {
            int32_t magnitude = Magnitude();

            if(magnitude != 0)
            {
                return Vector3Int(x / magnitude, y / magnitude, z / magnitude);
            }

            return Vector3Int(0, 0, 0);
        };

        inline int32_t Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        inline int32_t SqrMagnitude() const
        {
            return x * x + y * y + z * z;
        }

        inline static int32_t Dot(Vector3Int a, Vector3Int b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z; 
        };

        inline static Vector3Int Cross(Vector3Int a, Vector3Int b)
        {
            return Vector3Int(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }
    };
}