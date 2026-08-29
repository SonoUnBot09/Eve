#pragma once

#include <graphics/GraphicsCore.hpp>
#include <graphics/ErrorManager.hpp>
#include <eve/graphics/Buffer.hpp>
#include <eve/graphics/Sampler.hpp>
#include <eve/graphics/Texture.hpp>

#include <graphics/Resources.hpp>

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanMapping.hpp"

namespace Eve::Graphics::Helpers
{
    static inline void AllocateGPUBuffer(BufferInfo bufferInfo, BufferObject& buffer)
    {
        VkBufferCreateInfo bufferCI
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = bufferInfo.Size,
            .usage = bufferInfo.Usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VmaAllocationCreateInfo bufferAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vmaCreateBuffer(GraphicsCore::Context.Allocator, &bufferCI, &bufferAllocInfo,
            &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo));
    }

    static inline void AllocateCPUBuffer(BufferInfo bufferInfo, BufferObject& buffer)
    {
        VkBufferCreateInfo bufferCI
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = bufferInfo.Size,
            .usage = bufferInfo.Usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VmaAllocationCreateInfo bufferAllocInfo
        {
            .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
        };

        VkResult result = vmaCreateBuffer(GraphicsCore::Context.Allocator, &bufferCI, &bufferAllocInfo,
            &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo);
    }

    static inline void AllocateSampler(SamplerInfo samplerInfo, SamplerObject& sampler)
    {
        VkSamplerCreateInfo samplerCI
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = GetVkFilterMode(samplerInfo.MagFilter),
            .minFilter = GetVkFilterMode(samplerInfo.MinFilter),
            .mipmapMode = GetVkMipmapMode(samplerInfo.MipmapMode),
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        VK_CHECK(vkCreateSampler(GraphicsCore::Context.Device, &samplerCI, nullptr, &sampler.Sampler));

    }

    static inline void AllocateTexture1D(TextureInfo1D textureInfo, TextureObject& texture, VkImageUsageFlags usage)
    {
        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType =VK_IMAGE_TYPE_1D,
            .format = format,
            .extent {.width = textureInfo.Width, .height = 1, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
            &texture.Image, &texture.Allocation, &texture.AllocationInfo));
        
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture.Image,
            .viewType = VK_IMAGE_VIEW_TYPE_1D,
            .format = format,
            .subresourceRange
            {
                .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &texture.ImageView));
    }

    static inline void AllocateTexture2D(TextureInfo2D textureInfo, TextureObject& texture, VkImageUsageFlags usage)
    {
        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent {.width = textureInfo.Width, .height = textureInfo.Width, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
            &texture.Image, &texture.Allocation, &texture.AllocationInfo));
        
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture.Image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .subresourceRange
            {
                .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &texture.ImageView));
    }

    static inline void AllocateTexture3D(TextureInfo3D textureInfo, TextureObject& texture, VkImageUsageFlags usage)
    {
        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_3D,
            .format = format,
            .extent {.width = textureInfo.Width, .height = textureInfo.Width, .depth = textureInfo.Depth},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
            &texture.Image, &texture.Allocation, &texture.AllocationInfo));
        
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture.Image,
            .viewType = VK_IMAGE_VIEW_TYPE_3D,
            .format = format,
            .subresourceRange
            {
                .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &texture.ImageView));
    }

    static inline void AllocateTextureCube(TextureInfo2D textureInfo, TextureObject& texture, VkImageUsageFlags usage)
    {
        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent {.width = textureInfo.Width, .height = textureInfo.Width, .depth = 1},
            .mipLevels = 1,
            .arrayLayers = 6,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
            &texture.Image, &texture.Allocation, &texture.AllocationInfo));
        
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = texture.Image,
            .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
            .format = format,
            .subresourceRange
            {
                .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 6
            }
        };

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &texture.ImageView));
    }
}