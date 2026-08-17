#include <cstdint>
#include <graphics/GraphicsCore.hpp>
#include "TransientResourcePool.hpp"
#include "EveSettings.hpp"
#include "graphics/RenderGraph.hpp"
#include "graphics/Resources.hpp"
#include "graphics/helpers/VulkanMapping.hpp"
#include <graphics/registers/ResourceRegistry.hpp>
#include <graphics/MemoryBin.hpp>

using namespace Eve::Graphics;

namespace 
{
    void CreateVkImageCreateInfo(const TextureInfo& textureInfo, VkImageCreateInfo& imageCI)
    {
        imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCI.imageType = GetVkImageType(textureInfo.TextureType);
        imageCI.format = GetVkImageFormat(textureInfo.Format);
        imageCI.extent.width = textureInfo.Width;
        imageCI.extent.height = textureInfo.Height;
        imageCI.extent.depth = textureInfo.Depth;
        imageCI.mipLevels = 1;
        imageCI.arrayLayers = textureInfo.ArrayLayers;
        imageCI.samples = GetVkImageSamplesCount(textureInfo.Sample);
        imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCI.usage = GetVkImageUsage(textureInfo.Usage);
        imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCI.queueFamilyIndexCount = 0;
        imageCI.pQueueFamilyIndices = nullptr;
        imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    void CreateVkBufferCreateInfo(const BufferInfo& bufferInfo, VkBufferCreateInfo& bufferCI)
    {
        bufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCI.size = bufferInfo.Size;
        bufferCI.usage = GetVkBufferUsage(bufferInfo.Usage);
        bufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCI.queueFamilyIndexCount = 0;
        bufferCI.pQueueFamilyIndices = nullptr;
    }

    uint32_t FindBestMemoryTypeIndex(const VkMemoryRequirements2& memoryRequirements)
    {
        bool isDedicatedGPU = GraphicsCore::Context.PhysicalDeviceInfo.isDedicated;
        int32_t score = INT32_MIN;
        int32_t memoryTypeIndex = 0;
        for(uint32_t i = 0; i < GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypeCount; i++)
        {
            bool isCompatibleBit = (memoryRequirements.memoryRequirements.memoryTypeBits & (1 << i)) != 0;

            if(!isCompatibleBit) { continue; }

            int32_t currentScore = 0;

            // Local Device?
            if(GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                currentScore += 500;
            }

            // Host Visible?
            if(isDedicatedGPU && (GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            {
                currentScore -= 200;
            }

            // Host Coherent?
            if(isDedicatedGPU && (GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                currentScore -= 150;
            }

            // Lazily Allocated?
            if((GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
            {
                currentScore -= 1000;
            }

            // Some kink of memory which is not DEVICE_LOCAL, HOST_VISIBLE and HOST_COHERENT
            if((GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags &
                ~(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) > 0 )
            {
                currentScore -= 5000;
            }

            if(currentScore > score)
            {
                memoryTypeIndex = i;
                score = currentScore;
            }
        }

        return memoryTypeIndex;
    };
}

void TransientResourcePool::AddTextureResource(const TextureInfo& textureInfo, const uint32_t Id, const uint32_t frameIndex)
{
    TextureResource resource
    {
        .TextureInfo = textureInfo,
        .Id = Id
    };

    transientTextures[frameIndex].push_back(resource);
}

void TransientResourcePool::AddBufferResource(const BufferInfo& bufferInfo, const uint32_t Id, const uint32_t frameIndex)
{
    BufferResource resource
    {
        .BufferInfo = bufferInfo,
        .Id = Id
    };

    transientBuffers[frameIndex].push_back(resource);
}

uint32_t TransientResourcePool::FindTexturePoolIndex(const TextureInfo& textureInfo, const uint32_t passesCount)
{
    for(uint32_t i = 0; i < texturePools.size(); i++)
    {
        TexturePool& pool = texturePools[i];

        bool isTheSame = textureInfo == pool.TextureInfo;

        if(isTheSame) { return i; }
    }

    // Texture Pool not found, need to create a new one
    // --- Texture Pool Creation ---

    VkImageCreateInfo imageCI {};
    CreateVkImageCreateInfo(textureInfo, imageCI);

    VkDeviceImageMemoryRequirements reqs
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS,
        .pCreateInfo = &imageCI
    };

    VkMemoryRequirements2 memoryRequirements { .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetDeviceImageMemoryRequirements(GraphicsCore::Context.Device, &reqs, &memoryRequirements);

    uint32_t memoryTypeIndex = FindBestMemoryTypeIndex(memoryRequirements);

    uint32_t bucketIndex = GetTexturesBucketIndex(memoryTypeIndex, passesCount);

    MemoryInfo memoryInfo
    {
        .Size = memoryRequirements.memoryRequirements.size,
        .Alignment = memoryRequirements.memoryRequirements.alignment,
        .BucketIndex = bucketIndex
    };

    TexturePool newPool
    {
        .TextureInfo = textureInfo,
        .MemoryInfo = memoryInfo,
        .Countdown = 5,
        .Textures = std::vector<TransientTextureObject>()
    };

    uint32_t poolIndex;
    if(texturePoolFreeSlots.empty())
    {
        texturePools.push_back(newPool);
        poolIndex = texturePools.size() - 1;
    }
    else 
    {
        poolIndex = texturePoolFreeSlots.back();
        texturePoolFreeSlots.pop_back();

        texturePools[poolIndex] = newPool;
    }

    return poolIndex;
}

uint32_t TransientResourcePool::FindBufferPoolIndex(const BufferInfo& bufferInfo, const uint32_t passesCount)
{
    for(uint32_t i = 0; i < bufferPools.size(); i++)
    {
        BufferPool& pool = bufferPools[i];

        bool isTheSame = bufferInfo == pool.BufferInfo;

        if(isTheSame) { return i; }
    }

    // Texture Pool not found, need to create a new one
    // --- Texture Pool Creation ---

    VkBufferCreateInfo bufferCI {};
    CreateVkBufferCreateInfo(bufferInfo, bufferCI);

    VkDeviceBufferMemoryRequirements reqs
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
        .pCreateInfo = &bufferCI
    };

    VkMemoryRequirements2 memoryRequirements { .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetDeviceBufferMemoryRequirements(GraphicsCore::Context.Device, &reqs, &memoryRequirements);

    uint32_t memoryTypeIndex = FindBestMemoryTypeIndex(memoryRequirements);

    uint32_t bucketIndex = GetTexturesBucketIndex(memoryTypeIndex, passesCount);

    MemoryInfo memoryInfo
    {
        .Size = memoryRequirements.memoryRequirements.size,
        .Alignment = memoryRequirements.memoryRequirements.alignment,
        .BucketIndex = bucketIndex
    };

    BufferPool newPool
    {
        .BufferInfo = bufferInfo,
        .MemoryInfo = memoryInfo,
        .Countdown = 5,
        .Buffers = std::vector<TransientBufferObject>()
    };

    uint32_t poolIndex;
    if(bufferPoolFreeSlots.empty())
    {
        bufferPools.push_back(newPool);
        poolIndex = bufferPools.size() - 1;
    }
    else 
    {
        poolIndex = bufferPoolFreeSlots.back();
        bufferPoolFreeSlots.pop_back();

        bufferPools[poolIndex] = newPool;
    }

    return poolIndex;
}

uint32_t TransientResourcePool::GetTexturesBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount)
{
    uint32_t bucketsCount = texturesMemoryTypeIndicies.size();
    for (uint32_t i = 0; i < bucketsCount; i++)
    {
        if(texturesMemoryTypeIndicies[i] == memoryTypeIndex)
        {
            return i;
        }
    }

    texturesMemoryTypeIndicies.push_back(memoryTypeIndex);
    uint32_t bucketIndex = bucketsCount;

    RenderGraph::AddTextureBucketPasses(passesCount);

    MemoryBucket memoryBucket
    {
        .IsActive = false
    };
    texturesMemoryBucket.push_back(memoryBucket);

    return bucketIndex;
}

uint32_t TransientResourcePool::GetBuffersBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount)
{
    uint32_t bucketsCount = buffersMemoryTypeIndicies.size();
    for (uint32_t i = 0; i < bucketsCount; i++)
    {
        if(buffersMemoryTypeIndicies[i] == memoryTypeIndex)
        {
            return i;
        }
    }

    buffersMemoryTypeIndicies.push_back(memoryTypeIndex);
    uint32_t bucketIndex = bucketsCount;

    RenderGraph::AddBufferBucketPasses(passesCount);

    MemoryBucket memoryBucket
    {
        .IsActive = false
    };
    buffersMemoryBucket.push_back(memoryBucket);

    return bucketIndex;
}

bool TransientResourcePool::ResizeTextureMemoryBucketIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
{
    if(peakSize == 0) { return true; }

    MemoryBucket& memoryBucket = GetTextureMemoryBucket(bucketIndex);

    if(memoryBucket.IsActive && memoryBucket.AllocationInfo.size >= peakSize)
    {
        return true;
    }

    MemoryBin::DestroyMemoryBucket(memoryBucket);

    std::vector<TexturePool>& pools = GetTexturePools();
    for(uint32_t poolIndex = 0; poolIndex < pools.size(); poolIndex++)
    {
        TexturePool& pool = pools[poolIndex];

        if(pool.MemoryInfo.BucketIndex != bucketIndex) { continue; }

        for(int32_t i = static_cast<int32_t>(pool.Textures.size()) - 1; i >= 0; i--)
        {
            MemoryBin::DestroyTransientTexture(pool.Textures[i], Eve::Settings::MAX_FRAMES_IN_FLIGHT);

            pool.Textures.pop_back();
        }
    }

    if(memoryBucket.IsActive)
    {
        memoryBucket.IsActive = false;
    }

    VkMemoryRequirements memReqs
    {
        .size = peakSize + Eve::Settings::transientTexturesStepPoolSize,
        .alignment = peakAlignment,
        .memoryTypeBits = (1u << GetTextureMemoryTypeIndex(bucketIndex))
    };

    VmaAllocationCreateInfo allocInfo
    {
        .usage = VMA_MEMORY_USAGE_UNKNOWN
    };

    VkResult result = vmaAllocateMemory(GraphicsCore::Context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.IsActive = true;

    return true;
}

bool TransientResourcePool::ResizeBufferMemoryBucketIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
{
    if(peakSize == 0) { return true; }

    MemoryBucket& memoryBucket = GetBufferMemoryBucket(bucketIndex);

    if(memoryBucket.IsActive && memoryBucket.AllocationInfo.size >= peakSize)
    {
        return true;
    }

    MemoryBin::DestroyMemoryBucket(memoryBucket);

    std::vector<BufferPool>& pools = GetBufferPools();
    for(uint32_t poolIndex = 0; poolIndex < pools.size(); poolIndex++)
    {
        BufferPool& pool = pools[poolIndex];

        if(pool.MemoryInfo.BucketIndex != bucketIndex) { continue; }

        for(int32_t i = static_cast<int32_t>(pool.Buffers.size()) - 1; i >= 0; i--)
        {
            MemoryBin::DestroyTransientBuffer(pool.Buffers[i], Eve::Settings::MAX_FRAMES_IN_FLIGHT);

            pool.Buffers.pop_back();
        }
    }

    if(memoryBucket.IsActive)
    {
        memoryBucket.IsActive = false;
    }

    VkMemoryRequirements memReqs
    {
        .size = peakSize + Eve::Settings::transientBuffersStepPoolSize,
        .alignment = peakAlignment,
        .memoryTypeBits = (1u << GetBufferMemoryTypeIndex(bucketIndex))
    };

    VmaAllocationCreateInfo allocInfo
    {
        .usage = VMA_MEMORY_USAGE_UNKNOWN
    };

    VkResult result = vmaAllocateMemory(GraphicsCore::Context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.IsActive = true;

    return true;
}

void TransientResourcePool::UpdateTexturesPool(const uint32_t frameIndex)
{
    std::vector<TextureResource>& textures = transientTextures[frameIndex];
    uint32_t texturesCount = textures.size();

    for(uint32_t i = 0; i < texturesCount; i++)
    {
        TextureResource& resource = textures[i];
        ResourceRegistry::FreeTransientTextureSlot(resource.Id);
        
        TexturePool& pool = GetTexturePool(resource.TexturePoolIndex);

        if(resource.PooledResource) { continue; }

        TransientTextureObject object
        {
            .Image = resource.Image,
            .ImageView = resource.ImageView,
            .Countdown = 10,
            .MemoryOffset = resource.MemoryOffset,
            .PooledResource = false
        };

        pool.Textures.push_back(object);
    }

    textures.clear();

    std::vector<TexturePool>& pools = GetTexturePools();
    uint32_t poolsCount = pools.size();

    for(int32_t i = static_cast<int32_t>(poolsCount) - 1; i >= 0; i--)
    {
        TexturePool& pool = pools[i];

        uint32_t resourcesCount = pool.Textures.size();

        // --- Destroy old unused resources ---
        for(int32_t resourceIndex = static_cast<int32_t>(resourcesCount) - 1; resourceIndex >= 0; resourceIndex--)
        {
            TransientTextureObject& object = pool.Textures[resourceIndex];

            if(object.PooledResource)
            {
                object.Countdown = 10;
            }
            else 
            {
                object.Countdown--;
            }

            object.PooledResource = false;

            if(object.Countdown == 0)
            {
                MemoryBin::DestroyTransientTexture(object, 0);

                pool.Textures.erase(pool.Textures.begin() + resourceIndex);
            }
        }

        if(resourcesCount > 0)
        {
            pool.Countdown = 5;
        }
        else 
        {
            pool.Countdown--;
        }

        if(pool.Countdown == 0)
        {
            texturePoolFreeSlots.push_back(i);
        }

    }
}

void TransientResourcePool::UpdateBuffersPool(const uint32_t frameIndex)
{
    std::vector<BufferResource>& buffers = transientBuffers[frameIndex];
    uint32_t buffersCount = buffers.size();

    for(uint32_t i = 0; i < buffersCount; i++)
    {
        BufferResource& resource = buffers[i];
        ResourceRegistry::FreeTransientBufferSlot(resource.Id);
        
        BufferPool& pool = GetBufferPool(resource.BufferPoolIndex);

        if(resource.PooledResource) { continue; }

        TransientBufferObject object
        {
            .Buffer = resource.Buffer,
            .Countdown = 10,
            .MemoryOffset = resource.MemoryOffset,
            .PooledResource = false
        };

        pool.Buffers.push_back(object);
    }

    buffers.clear();

    std::vector<BufferPool>& pools = GetBufferPools();
    uint32_t poolsCount = pools.size();

    for(int32_t i = static_cast<int32_t>(poolsCount) - 1; i >= 0; i--)
    {
        BufferPool& pool = pools[i];

        uint32_t resourcesCount = pool.Buffers.size();

        // --- Destroy old unused resources ---
        for(int32_t resourceIndex = static_cast<int32_t>(resourcesCount) - 1; resourceIndex >= 0; resourceIndex--)
        {
            TransientBufferObject& object = pool.Buffers[resourceIndex];

            if(object.PooledResource)
            {
                object.Countdown = 10;
            }
            else 
            {
                object.Countdown--;
            }

            object.PooledResource = false;

            if(object.Countdown == 0)
            {
                MemoryBin::DestroyTransientBuffer(object, 0);
                
                pool.Buffers.erase(pool.Buffers.begin() + resourceIndex);
            }
        }

        if(resourcesCount > 0)
        {
            pool.Countdown = 5;
        }
        else 
        {
            pool.Countdown--;
        }

        if(pool.Countdown == 0)
        {
            bufferPoolFreeSlots.push_back(i);
        }
    }
}