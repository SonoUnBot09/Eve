#include <eve/math/Quaternion.hpp>
#include <eve/math/Matrix4x4.hpp>

using namespace Eve::Math;

void Quaternion::ToMatrix4x4(const Quaternion& q, Matrix4x4& mat) 
{
    Quaternion norm = q.Normalized();
    float xx = norm.x * norm.x, yy = norm.y * norm.y, zz = norm.z * norm.z;
    float xy = norm.x * norm.y, xz = norm.x * norm.z, yz = norm.y * norm.z;
    float wx = norm.w * norm.x, wy = norm.w * norm.y, wz = norm.w * norm.z;
    mat.m[0]  = 1.0f - 2.0f * (yy + zz);
    mat.m[1]  = 2.0f * (xy + wz);
    mat.m[2]  = 2.0f * (xz - wy);
    mat.m[3]  = 0.0f;

    mat.m[4]  = 2.0f * (xy - wz);
    mat.m[5]  = 1.0f - 2.0f * (xx + zz);
    mat.m[6]  = 2.0f * (yz + wx);
    mat.m[7]  = 0.0f;

    mat.m[8]  = 2.0f * (xz + wy);
    mat.m[9]  = 2.0f * (yz - wx);
    mat.m[10] = 1.0f - 2.0f * (xx + yy);
    mat.m[11] = 0.0f;

    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
}