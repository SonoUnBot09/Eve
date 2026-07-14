#pragma once


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

            bool BuildGraphicsPipeline(PipelineInfo pipelineInfo,  PipelineHandle& handle);

        private:

            inline static std::vector<Pipeline> pipelines;
            inline static VkShaderModule CreateShaderModule(std::string path, shaderc_shader_kind kind);
    };
}