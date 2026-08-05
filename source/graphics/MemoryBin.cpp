#include "MemoryBin.hpp"
#include "EveSettings.hpp"
#include "GraphicsCore.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include "registers/ResourceRegistry.hpp"

using namespace Eve::Graphics;

void MemoryBin::DestroyPendingResources()
{

    if(isGPUInIdle)
    {
        isGPUInIdle = false;

        for(int32_t i = 0; i < buffersToDestroy.size(); i++)
        {
            buffersToDestroy[i].second = 0;
        }

        for(int32_t i = 0; i < texturesToDestroy.size(); i++)
        {
            texturesToDestroy[i].second = 0;
        }

        for(int32_t i = 0; i < samplersToDestroy.size(); i++)
        {
            samplersToDestroy[i].second = 0;
        }
    }

    // --- Buffer Destruction ---
    for(int32_t i = buffersToDestroy.size(); i >= 0; i--)
    {
        if(buffersToDestroy[i].second > 0)
        {
            buffersToDestroy[i].second--;
            continue;
        }

        BufferObject& buffer = buffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        buffersToDestroy.erase(buffersToDestroy.begin() + i);
    }

    // --- Texture Destruction ---
    for(int32_t i = texturesToDestroy.size(); i >= 0; i--)
    {
        if(texturesToDestroy[i].second > 0)
        {
            texturesToDestroy[i].second--;
            continue;
        }

        TextureObject& texture = texturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        texturesToDestroy.erase(texturesToDestroy.begin() + i);
    }

    // --- Sampler Destruction ---
    for(int32_t i = samplersToDestroy.size(); i >= 0; i--)
    {
        if(samplersToDestroy[i].second > 0)
        {
            samplersToDestroy[i].second--;
            continue;
        }

        SamplerObject& sampler = samplersToDestroy[i].first;

        vkDestroySampler(GraphicsCore::Context.Device, sampler.Sampler, nullptr);

        samplersToDestroy.erase(samplersToDestroy.begin() + i);
    }
}

void MemoryBin::DestroyEverythingNow()
{
    for(int32_t i = buffersToDestroy.size(); i >= 0; i--)
    {
        BufferObject& buffer = buffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        buffersToDestroy.erase(buffersToDestroy.begin() + i);
    }

    for(int32_t i = texturesToDestroy.size(); i >= 0; i--)
    {
        TextureObject& texture = texturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        texturesToDestroy.erase(texturesToDestroy.begin() + i);
    }

    for(int32_t i = samplersToDestroy.size(); i >= 0; i--)
    {
        SamplerObject sampler = MemoryRegistry::samplers[i];

        vkDestroySampler(GraphicsCore::Context.Device, sampler.Sampler, nullptr);
    }

    for(uint32_t i = 0; i < MemoryRegistry::textures.size(); i++)
    {
        if(ResourceRegistry::persistentTextures[i] == false) { continue; }

        TextureObject texture = MemoryRegistry::textures[i];

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);
    }

    for(uint32_t i = 0; i < MemoryRegistry::buffers.size(); i++)
    {
        if(ResourceRegistry::persistentBuffers[i] == false) { continue; }

        BufferObject buffer = MemoryRegistry::buffers[i];

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);
    }


    for(uint32_t i = 0; i < MemoryRegistry::samplers.size(); i++)
    {
        bool skip = true;
        for(uint32_t freeSlotIndex = 0; freeSlotIndex < ResourceRegistry::samplerFreeSlots.size(); freeSlotIndex++)
        {
            if(freeSlotIndex == i) {skip = true; break;}
        }

        if(skip == true) { continue; }

        SamplerObject sampler = MemoryRegistry::samplers[i];

        vkDestroySampler(GraphicsCore::Context.Device, sampler.Sampler, nullptr);
    }

    MemoryRegistry::buffers.clear();
    MemoryRegistry::textures.clear();
    MemoryRegistry::samplers.clear();

    MemoryRegistry::buffersInfo.clear();
    MemoryRegistry::texturesInfo.clear();
    MemoryRegistry::samplersInfo.clear();
}

void MemoryBin::DestroyBuffer(BufferObject buffer)
{
    buffersToDestroy.push_back(std::pair{buffer, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroyTexture(TextureObject texture)
{
    texturesToDestroy.push_back(std::pair{texture, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroySampler(SamplerObject sampler)
{
    samplersToDestroy.push_back(std::pair{sampler, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}