#pragma once

#include <cstdint>
#include <eve/graphics/Geometry.hpp>
#include <eve/graphics/Texture.hpp>
#include <string>

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