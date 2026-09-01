#pragma once

#include <cstdint>
#include <eve/graphics/Geometry.hpp>
#include <eve/graphics/Texture.hpp>
#include <string>

#include <eve/math/Vector2.hpp>
#include <eve/math/Vector2Int.hpp>
#include <eve/math/Vector3.hpp>
#include <eve/math/Vector3Int.hpp>
#include <eve/math/Vector4.hpp>
#include <eve/math/Vector4Int.hpp>
#include <eve/math/Matrix4x4.hpp>

using namespace Eve::Math;

namespace Eve::Graphics
{
    struct ShaderHandle
    {
        uint32_t Id;

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

        uint32_t GetMaterialUBOId();
    };

    struct ShaderInfo
    {
        std::string ShaderModule;

        // Geometry
        enum Topology Topology = Topology::TOPOLOGY_TRIANGLE_LIST;
        enum PolygonMode PolygonMode = PolygonMode::POLYGON_MODE_FILL;
        enum CullMode CullMode = CullMode::CULL_MODE_BACK;
        float LineWidth = 1; 

        // Depth Stencil
        bool DepthTest = true, DepthWrite = true, StencilTest = false;;
        enum DepthTest CompareOp = DepthTest::DEPTH_COMPARE_LESS;

        // Color & Depth Formats
        Format ColorFormat, DepthFormat;
    };
}