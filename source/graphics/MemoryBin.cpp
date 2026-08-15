#include "MemoryBin.hpp"
#include "EveSettings.hpp"
#include "GraphicsCore.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include "registers/ResourceRegistry.hpp"
#include "registers/TransientResourcePool.hpp"

using namespace Eve::Graphics;

void MemoryBin::DestroyPendingResources()
{
    // --- Persistent Buffer Destruction ---
    for(int32_t i = static_cast<int32_t>(persistentBuffersToDestroy.size()) - 1; i >= 0; i--)
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
    for(int32_t i = static_cast<int32_t>(persistentTexturesToDestroy.size()) - 1; i >= 0; i--)
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
    for(int32_t i = static_cast<int32_t>(persistentSamplersToDestroy.size()) - 1; i >= 0; i--)
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
    for(int32_t i = static_cast<int32_t>(transientBuffersToDestroy.size()) - 1; i >= 0; i--)
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
    for(int32_t i = static_cast<int32_t>(transientTexturesToDestroy.size()) - 1; i >= 0; i--)
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

    // --- Transient Memory Buckets Destruction ---
    for(int32_t i = static_cast<int32_t>(memoryBucketsToDestroy.size()) - 1; i >= 0; i--)
    {
        std::pair<MemoryBucket, uint32_t>& memoryBucket = memoryBucketsToDestroy[i];

        if(memoryBucket.second > 0)
        {
            memoryBucket.second--;
            continue;
        }

        // No need to free memory
        if(!memoryBucket.first.IsActive) { memoryBucketsToDestroy.erase(memoryBucketsToDestroy.begin() + i); continue; }

        vmaFreeMemory(GraphicsCore::Context.Allocator, memoryBucket.first.Allocation);

        memoryBucketsToDestroy.erase(memoryBucketsToDestroy.begin() + i);
    }
}

void MemoryBin::DestroyAllPendingResources()
{
    // --- Persistent Buffer Destruction ---
    for(int32_t i = static_cast<int32_t>(persistentBuffersToDestroy.size()) - 1; i >= 0; i--)
    {
        BufferObject& buffer = persistentBuffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        persistentBuffersToDestroy.erase(persistentBuffersToDestroy.begin() + i);
    }

    // --- Persistent Texture Destruction ---
    for(int32_t i = static_cast<int32_t>(persistentTexturesToDestroy.size()) - 1; i >= 0; i--)
    {
        TextureObject& texture = persistentTexturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        persistentTexturesToDestroy.erase(persistentTexturesToDestroy.begin() + i);
    }

    // --- Sampler Destruction ---
    for(int32_t i = static_cast<int32_t>(persistentSamplersToDestroy.size()) - 1; i >= 0; i--)
    {
        SamplerObject& sampler = persistentSamplersToDestroy[i].first;

        vkDestroySampler(GraphicsCore::Context.Device, sampler.Sampler, nullptr);

        persistentSamplersToDestroy.erase(persistentSamplersToDestroy.begin() + i);
    }

    // --- Transient Buffer Destruction ---
    for(int32_t i = static_cast<int32_t>(transientBuffersToDestroy.size()) - 1; i >= 0; i--)
    {
        TransientBufferObject& buffer = transientBuffersToDestroy[i].first;

        vkDestroyBuffer(GraphicsCore::Context.Device, buffer.Buffer, nullptr);

        transientBuffersToDestroy.erase(transientBuffersToDestroy.begin() + i);
    }

    // --- Transient Texture Destruction ---
    for(int32_t i = static_cast<int32_t>(transientTexturesToDestroy.size()) - 1; i >= 0; i--)
    {
        TransientTextureObject& texture = transientTexturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vkDestroyImage(GraphicsCore::Context.Device, texture.Image, nullptr);

        transientTexturesToDestroy.erase(transientTexturesToDestroy.begin() + i);
    }

    // --- Transient Memory Buckets Destruction ---
    for(int32_t i = static_cast<int32_t>(memoryBucketsToDestroy.size()) - 1; i >= 0; i--)
    {
        MemoryBucket& memoryBucket = memoryBucketsToDestroy[i].first;

        // No need to free memory
        if(!memoryBucket.IsActive) { memoryBucketsToDestroy.erase(memoryBucketsToDestroy.begin() + i); continue; }

        vmaFreeMemory(GraphicsCore::Context.Allocator, memoryBucket.Allocation);

        memoryBucketsToDestroy.erase(memoryBucketsToDestroy.begin() + 1);
    }
}

void MemoryBin::DestroyEverything()
{
    // --- Persistent Textures ---
    for(uint32_t i = 0; i < MemoryRegistry::textures.size(); i++)
    {
        if(ResourceRegistry::persistentTextures[i] == false) { continue; }

        TextureObject texture = MemoryRegistry::textures[i];

        persistentTexturesToDestroy.push_back(std::pair{texture, 0});
    }

    // --- Persistent Buffers ---
    for(uint32_t i = 0; i < MemoryRegistry::buffers.size(); i++)
    {
        if(ResourceRegistry::persistentBuffers[i] == false) { continue; }

        BufferObject buffer = MemoryRegistry::buffers[i];

        persistentBuffersToDestroy.push_back(std::pair{buffer, 0});
    }

    // --- Persistent Samplers ---
    for(uint32_t i = 0; i < MemoryRegistry::samplers.size(); i++)
    {
        if(ResourceRegistry::persistentSamplers[i] == false) { continue; }

        SamplerObject sampler = MemoryRegistry::samplers[i];

        persistentSamplersToDestroy.push_back(std::pair{sampler, 0});
    }

    // --- Persistent Clear ---
    MemoryRegistry::buffers.clear();
    MemoryRegistry::textures.clear();
    MemoryRegistry::samplers.clear();

    MemoryRegistry::buffersInfo.clear();
    MemoryRegistry::texturesInfo.clear();
    MemoryRegistry::samplersInfo.clear();

    // --- Transient Textures & Buffers ---
    for(uint32_t frameIndex = 0; frameIndex < Eve::Settings::MAX_FRAMES_IN_FLIGHT; frameIndex++)
    {

        std::vector<TextureResource>& textures = TransientResourcePool::transientTextures[frameIndex];

        for(uint32_t i = 0; i < TransientResourcePool::transientTextures[frameIndex].size(); i++)
        {
            for(uint32_t textureIndex = 0; textureIndex < textures.size(); textureIndex++)
            {
                TextureResource& texture = textures[textureIndex];

                if(texture.PooledResource) { continue; }

                TransientTextureObject object
                {
                    .Image = texture.Image,
                    .ImageView = texture.ImageView
                };

                transientTexturesToDestroy.push_back(std::pair{object, 0});
            }
        }

        std::vector<BufferResource>& buffers = TransientResourcePool::transientBuffers[frameIndex];
        
        for(uint32_t i = 0; i < TransientResourcePool::transientBuffers[frameIndex].size(); i++)
        {
            for(uint32_t bufferIndex = 0; bufferIndex < buffers.size(); bufferIndex++)
            {
                BufferResource& buffer = buffers[bufferIndex];

                if(buffer.PooledResource) { continue; }

                TransientBufferObject object
                {
                    .Buffer = buffer.Buffer
                };

                transientBuffersToDestroy.push_back(std::pair{object, 0});
            }
        }

        TransientResourcePool::transientTextures[frameIndex].clear();
        TransientResourcePool::transientBuffers[frameIndex].clear();
    }

    // --- Transient Texture Pools ---
    for(uint32_t texturePool = 0; texturePool < TransientResourcePool::texturePools.size(); texturePool++)
    {
        TexturePool& pool = TransientResourcePool::texturePools[texturePool];

        for(uint32_t i = 0; i < pool.Textures.size(); i++)
        {
            transientTexturesToDestroy.push_back(std::pair{pool.Textures[i], 0});
        }

        pool.Textures.clear();
    }
    TransientResourcePool::texturePools.clear();

    // --- Transient Buffer Pools ---
    for(uint32_t bufferPool = 0; bufferPool < TransientResourcePool::bufferPools.size(); bufferPool++)
    {
        BufferPool& pool = TransientResourcePool::bufferPools[bufferPool];

        for(uint32_t i = 0; i < pool.Buffers.size(); i++)
        {
            transientBuffersToDestroy.push_back(std::pair{pool.Buffers[i], 0});
        }

        pool.Buffers.clear();
    }
    TransientResourcePool::bufferPools.clear();

    // --- Transient Buffer Memory Buckets ---
    for(uint32_t bufferMemoryBucket = 0; bufferMemoryBucket < TransientResourcePool::buffersMemoryBucket.size(); bufferMemoryBucket++)
    {
        MemoryBucket& memoryBucket = TransientResourcePool::buffersMemoryBucket[bufferMemoryBucket];

        DestroyMemoryBucket(memoryBucket);
    }
    TransientResourcePool::buffersMemoryBucket.clear();

    // --- Transient Texture Memory Bucket ---
    for(uint32_t textureMemoryBucket = 0; textureMemoryBucket < TransientResourcePool::texturesMemoryBucket.size(); textureMemoryBucket++)
    {
        MemoryBucket& memoryBucket = TransientResourcePool::texturesMemoryBucket[textureMemoryBucket];

        DestroyMemoryBucket(memoryBucket);
    }
    TransientResourcePool::texturesMemoryBucket.clear();

    // --- Transient Clear ---
    ResourceRegistry::persistentTextures.clear();
    ResourceRegistry::persistentBuffers.clear();
    ResourceRegistry::persistentSamplers.clear();
    ResourceRegistry::transientTextures.clear();
    ResourceRegistry::transientBuffers.clear();

    ResourceRegistry::textureResourcesPeakIndex = 0;
    ResourceRegistry::samplerResourcesPeakIndex = 0;
    ResourceRegistry::bufferResourcesPeakIndex = 0;

    ResourceRegistry::textureGenerations.clear();
    ResourceRegistry::samplerGenerations.clear();
    ResourceRegistry::bufferGenerations.clear();

    ResourceRegistry::textureFreeSlots.clear();
    ResourceRegistry::samplerFreeSlots.clear();
    ResourceRegistry::bufferFreeSlots.clear();

    // --- Destroy All Resources ---
    DestroyAllPendingResources();
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
