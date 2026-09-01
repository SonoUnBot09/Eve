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
    MaterialProperties materialProperties {};

    PipelineBuilder::BuildGraphicsPipeline(shaderInfo, shaderObject, materialProperties);

    shaderObjects.push_back(shaderObject);

    std::vector<BufferHandle> uboHandles; uboHandles.reserve(Eve::Settings::MAX_FRAMES_IN_FLIGHT);

    for(uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        BufferHandle handle = MemoryRegistry::CreateUBOBuffer(1024 * 16); //16 KB
        uboHandles.push_back(handle);
    }

    properties.emplace_back(PropertiesUBOs{materialProperties, uboHandles});

    return ShaderHandle{.Id = static_cast<uint32_t>(shaderObjects.size() - 1)};
}

void ShaderRegistry::UpdateMaterial(std::string& paramName, void* value, uint32_t paramSize, ShaderHandle handle)
{
    PropertiesUBOs& material = properties[handle.Id];

    std::vector<std::string>& paramNames = material.Properties.names;

    bool found = false;
    uint32_t index = 0;
    for(uint32_t i = 0; i < paramNames.size(); i++)
    {
        if(paramName == paramNames[i])
        {   
            found = true;
            index = i;
            break;
        }
    }

    if(!found) { return; }

    size_t offset = material.Properties.offsets[index];

    memcpy
    (
        static_cast<std::byte*>(material.MaterialData.data()) + offset,
        value,
        paramSize
    );

    material.countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT;
}
 
void ShaderRegistry::UploadMaterials()
{
    TransferPass pass {};
    bool upload = false;
    for (uint32_t i = 0; i < properties.size(); i++)
    {
        PropertiesUBOs& material = properties[i];

        if(material.countdown == 0) { continue; }

        BufferHandle dstBuffer = material.UBOs[GraphicsCore::GetFrameIndex()];

        pass.UploadBuffer
        (
            material.MaterialData.data(),
            dstBuffer,
            1024 * 16,
            0
        );

        material.countdown--;

        upload = true;
    }

    if(upload)
    {
        RenderGraph::AddPass(pass, 1);
    }
}

void ShaderRegistry::DestroyAllShaders()
{
    VkPipelineLayout graphicsPipelineLayout {};
    if(PipelineBuilder::GetGraphicsPipelineLayout(graphicsPipelineLayout))
    {
        vkDestroyPipelineLayout(GraphicsCore::Context.Device, graphicsPipelineLayout, nullptr);
    }

    for(uint32_t i = 0; i < shaderObjects.size(); i++)
    {
        GraphicsShaderObject& shader = shaderObjects[i];

        vkDestroyPipeline(GraphicsCore::Context.Device, shader.Pipeline, nullptr);

        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.VertexModule, nullptr);
        vkDestroyShaderModule(GraphicsCore::Context.Device, shader.FragmentModule, nullptr);
    }
}
