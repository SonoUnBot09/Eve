#pragma once

#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <vector>
#include <string>

#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Geometry.hpp>

namespace Eve::Graphics
{
    struct Pipeline
    {
        VkPipelineLayout Layout;
        VkPipeline Pipeline;
    };

    struct PipelineInfo
    {
        std::string ShaderPath;

        // Push Constant Offsets & Strides
        uint32_t VertOffset, VertStride, FragOffset, FragStride;

        // Vertex Attributes
        std::vector<Format> VerticesAttributes;

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