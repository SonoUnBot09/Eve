#pragma once

#include <cstdint>
#include <Eve/graphics/Geometry.hpp>
#include <vector>
#include <Eve/graphics/Texture.hpp>
#include <string>

namespace Eve::Graphics
{
    struct ShaderHandle
    {
        uint32_t Id;
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

        // Multi Sampling
        TextureSample samplesCount = TextureSample::SAMPLE_1;

        // Color & Depth Formats
        Format ColorFormat, DepthFormat;
    };
}