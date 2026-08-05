#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <Eve/graphics/PassModule.hpp>

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