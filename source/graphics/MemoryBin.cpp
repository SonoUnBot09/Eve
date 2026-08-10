#include "MemoryBin.hpp"
#include "Eve/Debug.hpp"
#include "EveSettings.hpp"
#include "GraphicsCore.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include "registers/ResourceRegistry.hpp"

using namespace Eve::Graphics;

void MemoryBin::DestroyPendingResources()
{
    // --- Persistent Buffer Destruction ---
    for(int32_t i = persistentBuffersToDestroy.size() - 1; i >= 0; i--)
    {
        if(persistentBuffersToDestroy[i].second > 0)
        {
            persistentBuffersToDestroy[i].second--;
            continue;
        }

        BufferObject& buffer = persistentBuffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        persistentBuffersToDestroy.erase(persistentBuffersToDestroy.begin() + i);
    }

    // --- Persistent Texture Destruction ---
    for(int32_t i = persistentTexturesToDestroy.size() - 1; i >= 0; i--)
    {
        if(persistentTexturesToDestroy[i].second > 0)
        {
            persistentTexturesToDestroy[i].second--;
            continue;
        }

        TextureObject& texture = persistentTexturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        persistentTexturesToDestroy.erase(persistentTexturesToDestroy.begin() + i);
    }

    // --- Sampler Destruction ---
    for(int32_t i = persistentSamplersToDestroy.size() - 1; i >= 0; i--)
    {
        if(persistentSamplersToDestroy[i].second > 0)
        {
            persistentSamplersToDestroy[i].second--;
            continue;
        }

        SamplerObject& sampler = persistentSamplersToDestroy[i].first;

        vkDestroySampler(GraphicsCore::Context.Device, sampler.Sampler, nullptr);

        persistentSamplersToDestroy.erase(persistentSamplersToDestroy.begin() + i);
    }

    // --- Transient Buffer Destruction ---
    for(int32_t i = transientBuffersToDestroy.size() - 1; i >= 0; i--)
    {
        if(transientBuffersToDestroy[i].second > 0)
        {
            transientBuffersToDestroy[i].second--;
            continue;
        }

        TransientBufferObject& buffer = transientBuffersToDestroy[i].first;

        vkDestroyBuffer(GraphicsCore::Context.Device, buffer.Buffer, nullptr);

        transientBuffersToDestroy.erase(transientBuffersToDestroy.begin() + i);
    }

    // --- Transient Texture Destruction ---
    for(int32_t i = transientTexturesToDestroy.size() - 1; i >= 0; i--)
    {
        if(transientTexturesToDestroy[i].second > 0)
        {
            transientTexturesToDestroy[i].second--;
            continue;
        }

        TransientTextureObject& texture = transientTexturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vkDestroyImage(GraphicsCore::Context.Device, texture.Image, nullptr);

        transientTexturesToDestroy.erase(transientTexturesToDestroy.begin() + i);
    }

    for(int32_t i = memoryBucketsToDestroy.size() - 1; i >= 0; i--)
    {
        std::pair<MemoryBucket, uint32_t>& memoryBucket = memoryBucketsToDestroy[i];

        if(memoryBucket.second > 0)
        {
            memoryBucket.second--;
            continue;
        }

        vmaFreeMemory(GraphicsCore::Context.Allocator, memoryBucket.first.Allocation);

        memoryBucketsToDestroy.erase(memoryBucketsToDestroy.begin() + 1);
    }
}

void MemoryBin::FlushAllPendingResources()
{
    // TODO
    for(int32_t i = persistentBuffersToDestroy.size() - 1; i >= 0; i--)
    {
        BufferObject& buffer = persistentBuffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        persistentBuffersToDestroy.erase(persistentBuffersToDestroy.begin() + i);
    }

    for(int32_t i = persistentTexturesToDestroy.size() - 1; i >= 0; i--)
    {
        TextureObject& texture = persistentTexturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        persistentTexturesToDestroy.erase(persistentTexturesToDestroy.begin() + i);
    }

    for(int32_t i = persistentSamplersToDestroy.size() - 1; i >= 0; i--)
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

void MemoryBin::DestroyPersistentBuffer(BufferObject& buffer)
{
    persistentBuffersToDestroy.push_back(std::pair{buffer, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroyPersistentTexture(TextureObject& texture)
{
    persistentTexturesToDestroy.push_back(std::pair{texture, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroyPersistentSampler(SamplerObject& sampler)
{
    persistentSamplersToDestroy.push_back(std::pair{sampler, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroyTransientBuffer(TransientBufferObject& buffer, uint32_t countdown)
{
    transientBuffersToDestroy.push_back(std::pair{buffer, countdown});
}

void MemoryBin::DestroyTransientTexture(TransientTextureObject& texture, uint32_t countdown)
{
    transientTexturesToDestroy.push_back(std::pair{texture, countdown});
}

void MemoryBin::DestroyMemoryBucket(MemoryBucket& memoryBucket)
{
    memoryBucketsToDestroy.push_back(std::pair{memoryBucket, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}
