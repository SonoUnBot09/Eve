#pragma once

#include <cstdint>

#include <string>
#include <eve/graphics/ShaderHandle.hpp>

#include <glm/glm.hpp>

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
        void SetVector2(std::string paramName, glm::vec2 value);
        void SetVector2Int(std::string paramName, glm::ivec2 value);
        void SetVector3(std::string paramName, glm::vec3 value);
        void SetVector3Int(std::string paramName, glm::ivec3 value);
        void SetVector4(std::string paramName, glm::vec4 value);
        void SetVector4Int(std::string paramName, glm::ivec4 value);
    };
}