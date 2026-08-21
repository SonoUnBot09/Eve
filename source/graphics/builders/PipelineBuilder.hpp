#pragma once


#include <vector>

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <graphics/helpers/VulkanMapping.hpp>
#include <graphics/builders/SlangCompiler.hpp>

#include "ShaderObject.hpp"
#include <eve/graphics/ShaderHandle.hpp>

namespace Eve::Graphics
{
    class PipelineBuilder
    {
        public:

            static bool Initialize();
            static bool GetGraphicsPipelineLayout(VkPipelineLayout& graphicsPipelineLayout);
            inline static VkPipelineLayout GetGraphicsPipelineLayout() { return graphicsPipelineLayout; }
            static bool BuildGraphicsPipeline(ShaderInfo shaderInfo,  GraphicsShaderObject& pipeline);

        private:

            inline static VkShaderModule CreateVertexModule(ShaderBytecode& input);
            inline static VkShaderModule CreateFragmentModule(ShaderBytecode& input);
            inline static VkShaderModule CreateComputeModule(ShaderBytecode& input);

            inline static VkPipelineLayout graphicsPipelineLayout = nullptr;
    };
}