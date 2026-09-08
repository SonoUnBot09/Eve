#include <graphics/GraphicsCore.hpp>
#include "ShaderRegistry.hpp"
#include "MemoryRegistry.hpp"
#include "eve/graphics/Buffer.hpp"
#include "eve/graphics/Pass.hpp"
#include "graphics/builders/PipelineBuilder.hpp"
#include "graphics/builders/ShaderObject.hpp"
#include <graphics/registers/ShaderRegistry.hpp>
#include "graphics/RenderGraph.hpp"
#include <EveSettings.hpp>
#include <graphics/GraphicsCore.hpp>

using namespace Eve::Graphics;

ShaderHandle ShaderRegistry::CreateGraphicsShader(ShaderInfo shaderInfo)
{
    GraphicsShaderObject shaderObject {};
    MaterialProperties properties {};

    PipelineBuilder::BuildGraphicsPipeline(shaderInfo, shaderObject, properties);

    graphicsShaderObjects.push_back(shaderObject);

    materialProperties.push_back(properties);

    return ShaderHandle{.Id = static_cast<uint32_t>(graphicsShaderObjects.size() - 1)};
}

ComputeShaderHandle ShaderRegistry::CreateComputeShader(std::string shaderModule)
{
    ComputeShaderObject shaderObject {};

    PipelineBuilder::BuildComputePipeline(shaderModule, shaderObject);

    computeShaderObjects.push_back(shaderObject);

    return ComputeShaderHandle{.Id = static_cast<uint32_t>(computeShaderObjects.size() - 1)};
}

void ShaderRegistry::DestroyAllShaders()
{
    VkPipelineLayout graphicsPipelineLayout {};
    if(PipelineBuilder::GetGraphicsPipelineLayout(graphicsPipelineLayout))
    {
        vkDestroyPipelineLayout(GraphicsCore::Context.Device, graphicsPipelineLayout, nullptr);
    }

    VkPipelineLayout computePipelineLayout{};
    if(PipelineBuilder::GetComputePipelineLayout(computePipelineLayout))
    {
        vkDestroyPipelineLayout(GraphicsCore::Context.Device, computePipelineLayout, nullptr);
    }

    for(uint32_t i = 0; i < graphicsShaderObjects.size(); i++)
    {
        GraphicsShaderObject& shader = graphicsShaderObjects[i];

        vkDestroyPipeline(GraphicsCore::Context.Device, shader.Pipeline, nullptr);

        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.VertexModule, nullptr);
        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.FragmentModule, nullptr);
    }

    for(uint32_t i = 0; i < computeShaderObjects.size(); i++)
    {
        ComputeShaderObject& shader = computeShaderObjects[i];

        vkDestroyPipeline(GraphicsCore::Context.Device, shader.Pipeline, nullptr);

        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.ComputeShaderModule, nullptr);
    }
}