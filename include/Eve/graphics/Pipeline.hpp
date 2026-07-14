#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include <Eve/graphics/Image.hpp>
#include <Eve/graphics/Geometry.hpp>

namespace Eve::Graphics
{
    struct PipelineHandle
    {
        uint32_t Id;
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
        ImageSample samplesCount;

        // Color & Depth Formats
        Format ColorFormat, DepthFormat;

        PipelineInfo(std::string vertShaderPath, std::string fragShaderPath, Format colorFormat, Format depthFormat) : 
        VertShaderPath(vertShaderPath), FragShaderPath(fragShaderPath),
        VertOffset(0), VertStride(0), FragOffset(0), FragStride(0),
        VerticesAttributes({Format::FORMAT_R32G32B32_SFLOAT}),
        Topology(Topology::TOPOLOGY_TRIANGLE_LIST), PolygonMode(PolygonMode::POLYGON_MODE_FILL),
        CullMode(CullMode::CULL_MODE_BACK), LineWidth(1.0f), 
        DepthTest(true), DepthWrite(true), StencilTest(true), CompareOp(DepthTest::DEPTH_COMPARE_LESS),
        samplesCount(ImageSample::SAMPLE_1),
        ColorFormat(colorFormat), DepthFormat(depthFormat) {};

        
    };
}