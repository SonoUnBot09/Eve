#pragma once

#include <cmath>
#include <numbers>
#include "Vector3.hpp"

namespace Eve::Math
{
    class Matrix4x4;

    struct Quaternion
    {
        float x, y, z, w;

        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};

        Quaternion operator*(Quaternion a) const
        {
            float newX = w * a.x + x * a.w + y * a.z - z * a.y;
            float newY = w * a.y - x * a.z + y * a.w + z * a.x;
            float newZ = w * a.z + x * a.y - y * a.x + z * a.w;
            float newW = w * a.w - x * a.x - y * a.y - z * a.z;

            return Quaternion(newX, newY, newZ, newW);
        }

        bool operator==(Quaternion a) const
        {
            return x == a.x && y == a.y && z == a.z && w == a.w;
        }

        inline static float Dot(Quaternion a, Quaternion b) 
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        inline static Quaternion Identity()
        {
            return Quaternion(0, 0, 0 ,1);
        }

        inline static Quaternion Slerp(Quaternion a, Quaternion b, float t) 
        {
            float dot = Dot(a, b);
            
            if (dot < 0.0f) {
                b = Quaternion(-b.x, -b.y, -b.z, -b.w);
                dot = -dot;
            }

            if (dot > 0.9995f) {
                Quaternion quaternion(
                    a.x + t * (b.x - a.x),
                    a.y + t * (b.y - a.y),
                    a.z + t * (b.z - a.z),
                    a.w + t * (b.w - a.w));
                
                quaternion.Normalize();
                return quaternion;
            }

            float theta = std::acos(dot);
            float sinTheta = std::sin(theta);
            float af = std::sin((1.0f - t) * theta) / sinTheta;
            float bf = std::sin(t * theta) / sinTheta;

            return Quaternion(
                af * a.x + bf * b.x,
                af * a.y + bf * b.y,
                af * a.z + bf * b.z,
                af * a.w + bf * b.w
            );
        }

        inline static Quaternion Inverse(Quaternion a)
        {
            return Quaternion(-a.x, -a.y, -a.z, a.w);
        }

        inline static Vector3 RotateVector(Quaternion a, Vector3 v) 
        {
            Vector3 u = Vector3(a.x, a.y, a.z);
            float s = a.w;
            return 2.0f * Vector3::Dot(u, v) * u 
                + (s * s - Vector3::Dot(u, u)) * v 
                + 2.0f * s * Vector3::Cross(u, v);
        }

        inline static Quaternion FromEuler(Vector3 euler) 
        {
            float degToRad = std::numbers::pi / 180.0f;
            float pitch = euler.x * degToRad;
            float yaw   = euler.y * degToRad;
            float roll  = euler.z * degToRad;

            float cy = std::cos(yaw * 0.5f);
            float sy = std::sin(yaw * 0.5f);
            float cp = std::cos(pitch * 0.5f);
            float sp = std::sin(pitch * 0.5f);
            float cr = std::cos(roll * 0.5f);
            float sr = std::sin(roll * 0.5f);

            float w = cr * cp * cy + sr * sp * sy;
            float x = sr * cp * cy - cr * sp * sy;
            float y = cr * sp * cy + sr * cp * sy;
            float z = cr * cp * sy - sr * sp * sy;

            return Quaternion(x, y, z, w);
        }

        inline static Vector3 ToEuler(Quaternion a) {
            float sinr_cosp = 2.0f * (a.w * a.x + a.y * a.z);
            float cosr_cosp = 1.0f - 2.0f * (a.x * a.x + a.y * a.y);
            float roll = std::atan2(sinr_cosp, cosr_cosp);

            float sinp = 2.0f * (a.w * a.y - a.z * a.x);
            float pitch;
            if (std::abs(sinp) >= 1.0f)
                pitch = std::copysignf(std::numbers::pi / 2.0f, sinp);
            else
                pitch = std::asin(sinp);

            float siny_cosp = 2.0f * (a.w * a.z + a.x * a.y);
            float cosy_cosp = 1.0f - 2.0f * (a.y * a.y + a.z * a.z);
            float yaw = std::atan2(siny_cosp, cosy_cosp);

            float radToDeg = 180.0f / std::numbers::pi;
            return Vector3(pitch * radToDeg, yaw * radToDeg, roll * radToDeg);
        }

        void Normalize() 
        {
            float magnitude = Magnitude();
            if (magnitude > 0.0f) 
            {
                x = x / magnitude;
                y = y / magnitude;
                z = z / magnitude;
                w = w / magnitude;
            }
        }

        Quaternion Normalized() const
        {
            float magnitude = Magnitude();
            if (magnitude > 0.0f) 
            {
                return Quaternion(
                    x / magnitude,
                    y / magnitude,
                    z / magnitude,
                    w / magnitude
                );
            }

            return Quaternion::Identity();
        }

        float Magnitude() const
        {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        float SqrMagnitude() const
        {
            return x * x + y * y + z * z + w * w;
        }

        static void ToMatrix4x4(const Quaternion& q, Matrix4x4& mat);
    };
}