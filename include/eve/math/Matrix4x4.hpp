#pragma once

#include "Vector4.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"

namespace Eve::Math
{
    struct Matrix4x4
    {
        float m[16]{};

       Matrix4x4() = default;

        float& operator()(int row, int col)       { return m[row * 4 + col]; }
        float  operator()(int row, int col) const { return m[row * 4 + col]; }
        
        static Matrix4x4 Identity() 
        {
            Matrix4x4 mat;
            mat.m[0] = 1.0f; mat.m[5] = 1.0f; mat.m[10] = 1.0f; mat.m[15] = 1.0f;
            return mat;
        }

        static Matrix4x4 Zero() 
        {
            return Matrix4x4();
        }

        Vector4 GetRow(int index) const 
        {
            int offset = index * 4;
            return { m[offset], m[offset + 1], m[offset + 2], m[offset + 3] };
        }

        Vector4 GetColumn(int index) const 
        {
            return { m[index], m[index + 4], m[index + 8], m[index + 12] };
        }

        void SetRow(int index, const Vector4& row) 
        {
            int offset = index * 4;
            m[offset]     = row.x;
            m[offset + 1] = row.y;
            m[offset + 2] = row.z;
            m[offset + 3] = row.w;
        }

        void SetColumn(int index, const Vector4& col) 
        {
            m[index]      = col.x;
            m[index + 4]  = col.y;
            m[index + 8]  = col.z;
            m[index + 12] = col.w;
        }

        static Matrix4x4 TRS(const Vector3& pos, const Quaternion& q, const Vector3& s) 
        {
            Matrix4x4 mat;
            Matrix4x4 R;
            Quaternion::ToMatrix4x4(q, R);

            mat.m[0] = R.m[0] * s.x; mat.m[1] = R.m[1] * s.y; mat.m[2]  = R.m[2] * s.z; mat.m[3]  = pos.x;
            mat.m[4] = R.m[4] * s.x; mat.m[5] = R.m[5] * s.y; mat.m[6]  = R.m[6] * s.z; mat.m[7]  = pos.y;
            mat.m[8] = R.m[8] * s.x; mat.m[9] = R.m[9] * s.y; mat.m[10] = R.m[10] * s.z; mat.m[11] = pos.z;
            mat.m[12] = 0.0f; mat.m[13] = 0.0f; mat.m[14] = 0.0f; mat.m[15] = 1.0f;

            return mat;
        }

        static Matrix4x4 Perspective(float fovRadians, float aspect, float zNear, float zFar)
        {
            Matrix4x4 res = Zero();
            float tanHalfFov = std::tan(fovRadians * 0.5f);

            res.m[0]  = 1.0f / (aspect * tanHalfFov);
            res.m[5]  = 1.0f / tanHalfFov;
            res.m[10] = zFar / (zFar - zNear);
            res.m[11] = -(zFar * zNear) / (zFar - zNear);
            res.m[14] = 1.0f; // W_clip = +1.0 * Z_view

            return res;
        }

        static Matrix4x4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
        {
            Matrix4x4 res = Zero();

            res.m[0]  = 2.0f / (right - left);
            res.m[5]  = 2.0f / (top - bottom);
            res.m[10] = 1.0f / (zFar - zNear);

            res.m[3]  = -(right + left) / (right - left);
            res.m[7]  = -(top + bottom) / (top - bottom);
            res.m[11] = -zNear / (zFar - zNear);
            res.m[15] = 1.0f;

            return res;
        }

        Vector3 MultiplyPoint(const Vector3& point) const 
        {
            float x = m[0] * point.x + m[1] * point.y + m[2]  * point.z + m[3];
            float y = m[4] * point.x + m[5] * point.y + m[6]  * point.z + m[7];
            float z = m[8] * point.x + m[9] * point.y + m[10] * point.z + m[11];
            float w = m[12]* point.x + m[13]* point.y + m[14] * point.z + m[15];
            
            w = 1.0f / w;
            return { x * w, y * w, z * w };
        }

        Vector3 MultiplyPoint3x4(const Vector3& point) const {
            return {
                m[0] * point.x + m[1] * point.y + m[2]  * point.z + m[3],
                m[4] * point.x + m[5] * point.y + m[6]  * point.z + m[7],
                m[8] * point.x + m[9] * point.y + m[10] * point.z + m[11]
            };
        }

        Vector3 MultiplyVector(const Vector3& vector) const 
        {
            return {
                m[0] * vector.x + m[1] * vector.y + m[2]  * vector.z,
                m[4] * vector.x + m[5] * vector.y + m[6]  * vector.z,
                m[8] * vector.x + m[9] * vector.y + m[10] * vector.z
            };
        }

        Matrix4x4 Transpose() const
        {
            Matrix4x4 t;
            for (int r = 0; r < 4; ++r) 
            {
                for (int c = 0; c < 4; ++c) 
                {
                    t(c, r) = (*this)(r, c);
                }
            }
            return t;
        }

        Matrix4x4 Inverse() const
        {
            Matrix4x4 inv;

            inv.m[0] = m[5]  * m[10] * m[15] - m[5]  * m[11] * m[14] - m[9]  * m[6]  * m[15] + 
                       m[9]  * m[7]  * m[14] + m[13] * m[6]  * m[11] - m[13] * m[7]  * m[10];

            inv.m[4] = -m[4]  * m[10] * m[15] + m[4]  * m[11] * m[14] + m[8]  * m[6]  * m[15] - 
                        m[8]  * m[7]  * m[14] - m[12] * m[6]  * m[11] + m[12] * m[7]  * m[10];

            inv.m[8] = m[4]  * m[9]  * m[15] - m[4]  * m[11] * m[13] - m[8]  * m[5]  * m[15] + 
                       m[8]  * m[7]  * m[13] + m[12] * m[5]  * m[11] - m[12] * m[7]  * m[9];

            inv.m[12] = -m[4]  * m[9]  * m[14] + m[4]  * m[10] * m[13] + m[8]  * m[5]  * m[14] - 
                         m[8]  * m[6]  * m[13] - m[12] * m[5]  * m[10] + m[12] * m[6]  * m[9];

            inv.m[1] = -m[1]  * m[10] * m[15] + m[1]  * m[11] * m[14] + m[9]  * m[2]  * m[15] - 
                        m[9]  * m[3]  * m[14] - m[13] * m[2]  * m[11] + m[13] * m[3]  * m[10];

            inv.m[5] = m[0]  * m[10] * m[15] - m[0]  * m[11] * m[14] - m[8]  * m[2]  * m[15] + 
                       m[8]  * m[3]  * m[14] + m[12] * m[2]  * m[11] - m[12] * m[3]  * m[10];

            inv.m[9] = -m[0]  * m[9]  * m[15] + m[0]  * m[11] * m[13] + m[8]  * m[1]  * m[15] - 
                        m[8]  * m[3]  * m[13] - m[12] * m[1]  * m[11] + m[12] * m[3]  * m[9];

            inv.m[13] = m[0]  * m[9]  * m[14] - m[0]  * m[10] * m[13] - m[8]  * m[1]  * m[14] + 
                        m[8]  * m[2]  * m[13] + m[12] * m[1]  * m[10] - m[12] * m[2]  * m[9];

            inv.m[2] = m[1]  * m[6]  * m[15] - m[1]  * m[7]  * m[14] - m[5]  * m[2]  * m[15] + 
                       m[5]  * m[3]  * m[14] + m[13] * m[2]  * m[7]  - m[13] * m[3]  * m[6];

            inv.m[6] = -m[0]  * m[6]  * m[15] + m[0]  * m[7]  * m[14] + m[4]  * m[2]  * m[15] - 
                        m[4]  * m[3]  * m[14] - m[12] * m[2]  * m[7]  + m[12] * m[3]  * m[6];

            inv.m[10] = m[0]  * m[5]  * m[15] - m[0]  * m[7]  * m[13] - m[4]  * m[1]  * m[15] + 
                        m[4]  * m[3]  * m[13] + m[12] * m[1]  * m[7]  - m[12] * m[3]  * m[5];

            inv.m[14] = -m[0]  * m[5]  * m[14] + m[0]  * m[6]  * m[13] + m[4]  * m[1]  * m[14] - 
                         m[4]  * m[2]  * m[13] - m[12] * m[1]  * m[6]  + m[12] * m[2]  * m[5];

            inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - 
                        m[5] * m[3] * m[10] - m[9] * m[2] * m[7]  + m[9] * m[3] * m[6];

            inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + 
                       m[4] * m[3] * m[10] + m[8] * m[2] * m[7]  - m[8] * m[3] * m[6];

            inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9]  + m[4] * m[1] * m[11] - 
                         m[4] * m[3] * m[9]  - m[8] * m[1] * m[7]  + m[8] * m[3] * m[5];

            inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9]  - m[4] * m[1] * m[10] + 
                        m[4] * m[2] * m[9]  + m[8] * m[1] * m[6]  - m[8] * m[2] * m[5];

            float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];

            if (det == 0.0f) 
                return Identity();

            float invDet = 1.0f / det;
            for (int i = 0; i < 16; ++i)
                inv.m[i] *= invDet;

            return inv;
        }

        Matrix4x4 operator*(const Matrix4x4& rhs) const 
        {
            Matrix4x4 res;
            for (int r = 0; r < 4; ++r) 
            {
                for (int c = 0; c < 4; ++c) 
                {
                    res(r, c) = m[r * 4 + 0] * rhs(0, c) +
                                m[r * 4 + 1] * rhs(1, c) +
                                m[r * 4 + 2] * rhs(2, c) +
                                m[r * 4 + 3] * rhs(3, c);
                }
            }
            return res;
        }
    };
}