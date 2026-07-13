#pragma once

#include "Eve/graphics/Geometry.hpp"
#include "Eve/graphics/Image.hpp"
#include <cstdint>
#include <vector>

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <graphics/api/vk/VulkanMapping.hpp>

#include <Eve/graphics/Pipeline.hpp>

namespace Eve::Graphics
{
    struct Pipeline
    {
        VkPipelineLayout Layout;
        VkPipeline Pipeline;
    };

    class PipelineBuilder
    {
        public:

            static void SetVertexAttributes(std::vector<Format> formats);
            static void SetupPushConstantData(uint32_t vertOffset, uint32_t vertStride, uint32_t fragOffset, uint32_t fragStride);
            static void SetGeometryModes(Topology topology, PolygonMode polygonMode, CullMode cullMode, float lineWidth);
            static void SetDepthStencil(bool depthTest, bool depthWrite, DepthTest compareOp, bool stencilTest);
            static void SetSamplesCount(ImageSample sample);
            static void SetFormats(Format colorFormat, Format depthFormat);

            bool BuildGraphicsPipeline(std::string vertexShaderPath, std::string fragShaderPath, PipelineHandle& handle);

        private:

            struct PipelineInfo
            {
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
                VkBool32 DepthTest, DepthWrite, StencilTest;
                enum DepthTest CompareOp;

                // Multi Sampling
                ImageSample samplesCount;

                // Color & Depth Formats
                Format ColorFormat, DepthFormat;
            };

            inline static PipelineInfo pipelineInfo;
            /*inline static PipelineInfo defaultPipelineInfo
            {
                0,
                0,
                0,
                0,
                {Format::FORMAT_R32G32B32_SFLOAT},
                Topology::TOPOLOGY_TRIANGLE_LIST,
                PolygonMode::POLYGON_MODE_FILL,
                CullMode::CULL_MODE_BACK,
                1.0f,
                VK_TRUE, VK_TRUE, VK_FALSE,
                DepthTest::DEPTH_COMPARE_LESS,
                ImageSample::SAMPLE_1,
                Format::FORMAT_UNDEFINED,
                Format::FORMAT_UNDEFINED
            };*/

            inline static std::vector<Pipeline> pipelines;
            inline static VkShaderModule CreateShaderModule(std::string path, shaderc_shader_kind kind);
    };
}