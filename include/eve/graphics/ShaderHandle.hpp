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