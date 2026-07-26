#pragma once


#include <vector>

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <graphics/VulkanMapping.hpp>

#include "Pipeline.hpp"

namespace Eve::Graphics
{
    class PipelineBuilder
    {
        public:

            bool BuildGraphicsPipeline(PipelineInfo pipelineInfo,  Pipeline& pipeline);

        private:

            inline static VkShaderModule CreateShaderModule(std::string path, shaderc_shader_kind kind);
    };
}