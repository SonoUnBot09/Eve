#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <eve/graphics/Pass.hpp>

namespace Eve::Graphics
{
    struct TextureObject
    {
        VkImage Image;
        VkImageView ImageView;
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
    };

    struct TransientTextureObject
    {
        VkImage Image;
        VkImageView ImageView;
        uint32_t Countdown;
        uint64_t MemoryOffset;
        bool PooledResource;
    };

    struct TextureInfo
    {
        TextureType TextureType;
        uint32_t Width, Height, Depth;
        uint32_t ArrayLayers;
        uint32_t MipLevels;

        Format Format;
        VkImageUsageFlags Usage;

        bool operator==(const TextureInfo& other) const 
        {
            return 
                TextureType == other.TextureType &&
                Width == other.Width &&
                Height == other.Height &&
                Depth == other.Depth &&
                ArrayLayers == other.ArrayLayers &&
                MipLevels == other.MipLevels &&
                Format == other.Format &&
                Usage == other.Usage;
        }
    };

    struct SamplerObject
    {
        VkSampler Sampler;
    };

    struct BufferObject
    {
        VkBuffer Buffer;
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
    };

    struct TransientBufferObject
    {
        VkBuffer Buffer;
        uint32_t Countdown;
        uint64_t MemoryOffset;
        bool PooledResource;
    };

    struct BufferInfo
    {
        uint64_t Size;
        VkBufferUsageFlags Usage = 0;

        bool operator==(const BufferInfo& other) const
        {
            return 
                Size == other.Size &&
                Usage == other.Usage;
        }
    };


    struct TextureState
    {
        VkPipelineStageFlags2 StageMask;
        VkAccessFlags2 AccessMask;
        VkImageLayout Layout;
        Usage Usage;
    };

    struct BufferState
    {
        VkPipelineStageFlags2 StageMask;
        VkAccessFlags2 AccessMask;
        Usage Usage;
    };

    struct CommandBufferHandle
    {
        uint32_t Id;
    };

    struct CommandPoolHandle
    {
        uint32_t Id;
    };

}