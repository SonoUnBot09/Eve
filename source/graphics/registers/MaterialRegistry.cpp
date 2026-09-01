#include "MaterialRegistry.hpp"
#include "ShaderRegistry.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include <EveSettings.hpp>
#include <graphics/RenderGraph.hpp>

using namespace Eve::Graphics;

MaterialHandle MaterialRegistry::CreateMaterial(ShaderHandle shader)
{
    std::vector<BufferHandle> uboHandles; uboHandles.reserve(Eve::Settings::MAX_FRAMES_IN_FLIGHT);

    for(uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        BufferHandle handle = MemoryRegistry::CreateUBOBuffer(1024 * 16); //16 KB
        uboHandles.push_back(handle);
    }

    materials.emplace_back(MaterialObject{uboHandles, std::array<std::byte, 16 * 1024>(), shader, 0});

    return MaterialHandle{static_cast<uint32_t>(materials.size() - 1)};
}

void MaterialRegistry::UpdateMaterial(std::string& paramName, void* value, uint32_t paramSize, MaterialHandle handle)
{
    MaterialObject& material = materials[handle.Id];

    MaterialProperties& properties = ShaderRegistry::GetMaterialProperties(material.Shader);
    std::vector<std::string>& paramNames = properties.names;

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

    size_t offset = properties.offsets[index];

    memcpy
    (
        static_cast<std::byte*>(material.MaterialData.data()) + offset,
        value,
        paramSize
    );

    material.Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT;
}

void MaterialRegistry::UploadMaterials()
{
    TransferPass pass {};
    bool upload = false;
    for (uint32_t i = 0; i < materials.size(); i++)
    {
        MaterialObject& material = materials[i];

        if(material.Countdown == 0) { continue; }

        BufferHandle dstBuffer = material.UBOs[GraphicsCore::GetFrameIndex()];

        pass.UploadBuffer
        (
            material.MaterialData.data(),
            dstBuffer,
            1024 * 16,
            0
        );

        material.Countdown--;

        upload = true;
    }

    if(upload)
    {
        RenderGraph::AddPass(pass, 1);
    }
}