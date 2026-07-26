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
        std::string VertShaderPath;
        std::string FragShaderPath;

        // Push Constant Offsets & Strides
        uint32_t VertOffset, VertStride, FragOffset, FragStride;

        // Vertex Attributes
        std::vector<Format> VerticesAttributes;

        // Geometry
        enum Topology Topology;
        enum PolygonMode PolygonMode;
        enum CullMode CullMode;
        float LineWidth; 

        // Depth Stencil
        bool DepthTest, DepthWrite, StencilTest;
        enum DepthTest CompareOp;

        // Multi Sampling
        TextureSample samplesCount;

        // Color & Depth Formats
        Format ColorFormat, DepthFormat;
    };
}