#pragma once

#include "eve/graphics/Texture.hpp"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <eve/graphics/PassModule.hpp>

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