#include "MemoryBin.hpp"
#include "EveSettings.hpp"
#include "GraphicsCore.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"

using namespace Eve::Graphics;

void MemoryBin::DestroyPendingResources()
{
    // --- Buffer Destruction ---
    for(uint32_t i = 0; i < buffersToDestroy.size(); i++)
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
    for(uint32_t i = 0; i < texturesToDestroy.size(); i++)
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
    for(uint32_t i = 0; i < samplersToDestroy.size(); i++)
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
    for(uint32_t i = 0; i < buffersToDestroy.size(); i++)
    {
        BufferObject& buffer = buffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        buffersToDestroy.erase(buffersToDestroy.begin() + i);
    }

    for(uint32_t i = 0; i < texturesToDestroy.size(); i++)
    {
        TextureObject& texture = texturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        texturesToDestroy.erase(texturesToDestroy.begin() + i);
    }

    for(uint32_t i = 0; i < MemoryRegistry::buffers.size(); i++)
    {
        bool skip = false;
        for(uint32_t freeSlotIndex = 0; freeSlotIndex < MemoryRegistry::bufferFreeSlots.size(); freeSlotIndex++)
        {
            if(freeSlotIndex == i) {skip = true; break;}
        }

        if(skip == true) { continue; }

        BufferObject buffer = MemoryRegistry::buffers[i];

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);
    }

    for(uint32_t i = 0; i < MemoryRegistry::textures.size(); i++)
    {
        bool skip = false;
        for(uint32_t freeSlotIndex = 0; freeSlotIndex < MemoryRegistry::imageFreeSlots.size(); freeSlotIndex++)
        {
            if(freeSlotIndex == i) {skip = true; break;}
        }

        if(skip == true) { continue; }

        TextureObject texture = MemoryRegistry::textures[i];

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);
    }

    for(uint32_t i = 0; i < MemoryRegistry::samplers.size(); i++)
    {
        bool skip = false;
        for(uint32_t freeSlotIndex = 0; freeSlotIndex < MemoryRegistry::samplerFreeSlots.size(); freeSlotIndex++)
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

    MemoryRegistry::bufferFreeSlots.clear();
    MemoryRegistry::imageFreeSlots.clear();
    MemoryRegistry::samplerFreeSlots.clear();
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