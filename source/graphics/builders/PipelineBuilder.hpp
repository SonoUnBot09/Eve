#pragma once


#include <vector>

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <graphics/VulkanMapping.hpp>
#include <graphics/builders/SlangCompiler.hpp>

#include "Pipeline.hpp"

namespace Eve::Graphics
{
    class PipelineBuilder
    {
        public:

            bool BuildGraphicsPipeline(PipelineInfo pipelineInfo,  Pipeline& pipeline);

        private:

            inline static VkShaderModule CreateVertexModule(ShaderBytecode& input);
            inline static VkShaderModule CreateFragmentModule(ShaderBytecode& input);
            inline static VkShaderModule CreateComputeModule(ShaderBytecode& input);
    };
}