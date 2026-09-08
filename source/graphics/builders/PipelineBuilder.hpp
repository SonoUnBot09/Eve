#pragma once

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include <graphics/helpers/VulkanMapping.hpp>
#include <graphics/builders/SlangCompiler.hpp>

#include "ShaderObject.hpp"
#include "SlangCompiler.hpp"
#include <eve/graphics/ShaderHandle.hpp>

namespace Eve::Graphics
{
    class PipelineBuilder
    {
        public:

            static void Initialize();
            
            static bool GetGraphicsPipelineLayout(VkPipelineLayout& graphicsPipelineLayout);
            static bool GetComputePipelineLayout(VkPipelineLayout& computePipelineLayout);

            inline static VkPipelineLayout GetGraphicsPipelineLayout() { return graphicsPipelineLayout; }
            inline static VkPipelineLayout GetComputePielineLayout() { return computePipelineLayout; }

            static bool BuildGraphicsPipeline(ShaderInfo shaderInfo,  GraphicsShaderObject& pipeline, MaterialProperties& properties);
            static bool BuildComputePipeline(std::string& shaderModule, ComputeShaderObject& pipeline);

        private:

            inline static VkShaderModule CreateVertexModule(ShaderBytecode& input);
            inline static VkShaderModule CreateFragmentModule(ShaderBytecode& input);
            inline static VkShaderModule CreateComputeModule(ShaderBytecode& input);

            inline static VkPipelineLayout graphicsPipelineLayout = nullptr;
            inline static VkPipelineLayout computePipelineLayout = nullptr;
    };
}