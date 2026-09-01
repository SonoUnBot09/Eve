#pragma once

#include <cstdint>

#include <string>

#include <eve/math/Vector2.hpp>
#include <eve/math/Vector2Int.hpp>
#include <eve/math/Vector3.hpp>
#include <eve/math/Vector3Int.hpp>
#include <eve/math/Vector4.hpp>
#include <eve/math/Vector4Int.hpp>
#include <eve/math/Matrix4x4.hpp>
#include <eve/graphics/ShaderHandle.hpp>

using namespace Eve::Math;

namespace Eve::Graphics 
{
    struct MaterialHandle
    {
        uint32_t Id;

        ShaderHandle GetShader();
        void SetShader(ShaderHandle handle);

        void SetUInt(std::string paramName, uint32_t value);
        void SetInt(std::string paramName, int32_t value);
        void SetFloat(std::string paramName, float value);
        void SetVector2(std::string paramName, Vector2 value);
        void SetVector2Int(std::string paramName, Vector2Int);
        void SetVector3(std::string paramName, Vector3 value);
        void SetVector3Int(std::string paramName, Vector3Int value);
        void SetVector4(std::string paramName, Vector4 value);
        void SetVector4Int(std::string paramName, Vector4Int value);
        void SetMatrix4x4(std::string paramName, Matrix4x4 value);

        uint32_t GetPropertiesUBOId();

    };
}