#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Eve::Graphics
{
    struct TextureObject
    {
        VkImage Image;
        VkImageView ImageView;
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
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
}