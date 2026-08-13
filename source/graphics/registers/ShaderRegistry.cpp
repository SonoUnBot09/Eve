#include <graphics/GraphicsCore.hpp>
#include "ShaderRegistry.hpp"
#include "graphics/builders/ShaderObject.hpp"
#include <graphics/registers/ShaderRegistry.hpp>

using namespace Eve::Graphics;

ShaderHandle ShaderRegistry::CreateGraphicsShader(ShaderInfo shaderInfo)
{
    GraphicsShaderObject shaderObject {};

    PipelineBuilder::BuildGraphicsPipeline(shaderInfo, shaderObject);

    shaderObjects.push_back(shaderObject);

    return ShaderHandle{.Id = static_cast<uint32_t>(shaderObjects.size() - 1)};
}

void ShaderRegistry::DestroyAllShaders()
{
    for(uint32_t i = 0; i < shaderObjects.size(); i++)
    {
        GraphicsShaderObject& shader = shaderObjects[i];

        vkDestroyPipelineLayout(GraphicsCore::Context.Device, shader.Layout, nullptr);
        vkDestroyPipeline(GraphicsCore::Context.Device, shader.Pipeline, nullptr);

        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.VertexModule, nullptr);
        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.FragmentModule, nullptr);
    }
}