#pragma once

#include <cstdint>
#include <vulkan/vulkan.h>
#include <Eve/graphics/Types.hpp>

namespace Eve::Graphics
{

    constexpr uint32_t bufferTypeConfigCount = 6;
    constexpr uint32_t imageUsageConfigCount = 7;

    // Buffers
    static constexpr VkBufferUsageFlags vkBufferTypeLut[]
    {
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
    };

    // Images
    static constexpr VkImageType vkImageTypeLUT[]
    {
        VK_IMAGE_TYPE_1D,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TYPE_3D
    };

    static constexpr VkImageViewType vkImageViewTypeLUT[]
    {
        VK_IMAGE_VIEW_TYPE_1D,
        VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_VIEW_TYPE_3D,
        VK_IMAGE_VIEW_TYPE_CUBE,
        VK_IMAGE_VIEW_TYPE_1D_ARRAY,
        VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
    };

    static constexpr VkImageUsageFlags vkImageUsageLUT[]
    {
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_STORAGE_BIT,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
    };

    static constexpr VkImageLayout vkImageLayoutLUT[]
    {
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_PREINITIALIZED
    };

    static constexpr VkFormat vkImageFormatLUT[]
    {
        VK_FORMAT_UNDEFINED,             // FORMAT_UNDEFINED
        VK_FORMAT_R8_UNORM,              // FORMAT_R8_UNORM
        VK_FORMAT_R8_SNORM,              // FORMAT_R8_SNORM
        VK_FORMAT_R8_UINT,               // FORMAT_R8_UINT
        VK_FORMAT_R8_SINT,               // FORMAT_R8_SINT
        VK_FORMAT_R8_SRGB,               // FORMAT_R8_SRGB
        VK_FORMAT_R8G8_UNORM,            // FORMAT_R8G8_UNORM
        VK_FORMAT_R8G8_SNORM,            // FORMAT_R8G8_SNORM
        VK_FORMAT_R8G8_UINT,             // FORMAT_R8G8_UINT
        VK_FORMAT_R8G8_SINT,             // FORMAT_R8G8_SINT
        VK_FORMAT_R8G8_SRGB,             // FORMAT_R8G8_SRGB
        VK_FORMAT_R8G8B8_UNORM,          // FORMAT_R8G8B8_UNORM
        VK_FORMAT_R8G8B8_SNORM,          // FORMAT_R8G8B8_SNORM
        VK_FORMAT_R8G8B8_UINT,           // FORMAT_R8G8B8_UINT
        VK_FORMAT_R8G8B8_SINT,           // FORMAT_R8G8B8_SINT
        VK_FORMAT_R8G8B8_SRGB,           // FORMAT_R8G8B8_SRGB
        VK_FORMAT_B8G8R8_UNORM,          // FORMAT_B8G8R8_UNORM
        VK_FORMAT_B8G8R8_SNORM,          // FORMAT_B8G8R8_SNORM
        VK_FORMAT_B8G8R8_UINT,           // FORMAT_B8G8R8_UINT
        VK_FORMAT_B8G8R8_SINT,           // FORMAT_B8G8R8_SINT
        VK_FORMAT_B8G8R8_SRGB,           // FORMAT_B8G8R8_SRGB
        VK_FORMAT_R8G8B8A8_UNORM,        // FORMAT_R8G8B8A8_UNORM
        VK_FORMAT_R8G8B8A8_SNORM,        // FORMAT_R8G8B8A8_SNORM
        VK_FORMAT_R8G8B8A8_UINT,         // FORMAT_R8G8B8A8_UINT
        VK_FORMAT_R8G8B8A8_SINT,         // FORMAT_R8G8B8A8_SINT
        VK_FORMAT_R8G8B8A8_SRGB,         // FORMAT_R8G8B8A8_SRGB
        VK_FORMAT_B8G8R8A8_UNORM,        // FORMAT_B8G8R8A8_UNORM
        VK_FORMAT_B8G8R8A8_SNORM,        // FORMAT_B8G8R8A8_SNORM
        VK_FORMAT_B8G8R8A8_UINT,         // FORMAT_B8G8R8A8_UINT
        VK_FORMAT_B8G8R8A8_SINT,         // FORMAT_B8G8R8A8_SINT
        VK_FORMAT_B8G8R8A8_SRGB,         // FORMAT_B8G8R8A8_SRGB
        VK_FORMAT_R16_UNORM,             // FORMAT_R16_UNORM
        VK_FORMAT_R16_SNORM,             // FORMAT_R16_SNORM
        VK_FORMAT_R16_UINT,              // FORMAT_R16_UINT
        VK_FORMAT_R16_SINT,              // FORMAT_R16_SINT
        VK_FORMAT_R16_SFLOAT,            // FORMAT_R16_SFLOAT
        VK_FORMAT_R16G16_UNORM,          // FORMAT_R16G16_UNORM
        VK_FORMAT_R16G16_SNORM,          // FORMAT_R16G16_SNORM
        VK_FORMAT_R16G16_UINT,           // FORMAT_R16G16_UINT
        VK_FORMAT_R16G16_SINT,           // FORMAT_R16G16_SINT
        VK_FORMAT_R16G16_SFLOAT,         // FORMAT_R16G16_SFLOAT
        VK_FORMAT_R16G16B16_UNORM,       // FORMAT_R16G16B16_UNORM
        VK_FORMAT_R16G16B16_SNORM,       // FORMAT_R16G16B16_SNORM
        VK_FORMAT_R16G16B16_UINT,        // FORMAT_R16G16B16_UINT
        VK_FORMAT_R16G16B16_SINT,        // FORMAT_R16G16B16_SINT
        VK_FORMAT_R16G16B16_SFLOAT,      // FORMAT_R16G16B16_SFLOAT
        VK_FORMAT_R16G16B16A16_UNORM,    // FORMAT_R16G16B16A16_UNORM
        VK_FORMAT_R16G16B16A16_SNORM,    // FORMAT_R16G16B16A16_SNORM
        VK_FORMAT_R16G16B16A16_UINT,     // FORMAT_R16G16B16A16_UINT
        VK_FORMAT_R16G16B16A16_SINT,     // FORMAT_R16G16B16A16_SINT
        VK_FORMAT_R16G16B16A16_SFLOAT,   // FORMAT_R16G16B16A16_SFLOAT
        VK_FORMAT_R32_UINT,              // FORMAT_R32_UINT
        VK_FORMAT_R32_SINT,              // FORMAT_R32_SINT
        VK_FORMAT_R32_SFLOAT,            // FORMAT_R32_SFLOAT
        VK_FORMAT_R32G32_UINT,           // FORMAT_R32G32_UINT
        VK_FORMAT_R32G32_SINT,           // FORMAT_R32G32_SINT
        VK_FORMAT_R32G32_SFLOAT,         // FORMAT_R32G32_SFLOAT
        VK_FORMAT_R32G32B32_UINT,        // FORMAT_R32G32B32_UINT
        VK_FORMAT_R32G32B32_SINT,        // FORMAT_R32G32B32_SINT
        VK_FORMAT_R32G32B32_SFLOAT,      // FORMAT_R32G32B32_SFLOAT
        VK_FORMAT_R32G32B32A32_UINT,     // FORMAT_R32G32B32A32_UINT
        VK_FORMAT_R32G32B32A32_SINT,     // FORMAT_R32G32B32A32_SINT
        VK_FORMAT_R32G32B32A32_SFLOAT,   // FORMAT_R32G32B32A32_SFLOAT
        VK_FORMAT_D16_UNORM,             // FORMAT_D16_UNORM
        VK_FORMAT_D32_SFLOAT,            // FORMAT_D32_SFLOAT
        VK_FORMAT_S8_UINT,               // FORMAT_S8_UINT
        VK_FORMAT_D16_UNORM_S8_UINT,     // FORMAT_D16_UNORM_S8_UINT
        VK_FORMAT_D24_UNORM_S8_UINT,     // FORMAT_D24_UNORM_S8_UINT
        VK_FORMAT_D32_SFLOAT_S8_UINT     // FORMAT_D32_SFLOAT_S8_UINT
    };


    // Buffers
    static inline VkBufferUsageFlags GetVkBufferType(BufferType type)
    {
        uint32_t bits = static_cast<uint32_t>(type);

        VkBufferUsageFlags bufferType = 0;
        for(uint32_t i = 0; i < bufferTypeConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            bufferType |= vkBufferTypeLut[i];
        }

        return bufferType;
    }

    // Images
    static inline VkImageType GetVkImageType(ImageType type)
    {
        return vkImageTypeLUT[static_cast<uint32_t>(type)];
    }
    static inline VkImageViewType GetVkImageViewType(ImageViewType type)
    {
        return vkImageViewTypeLUT[static_cast<uint32_t>(type)];
    }
    static inline VkImageUsageFlags GetVkImageUsage(ImageUsage usage)
    {
        uint32_t bits = static_cast<uint32_t>(usage);

        VkImageUsageFlags imageUsageFlag = 0;
        for(uint32_t i = 0; i < imageUsageConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            imageUsageFlag |= vkImageUsageLUT[i];
        }

        return imageUsageFlag;
    }
    static inline VkImageLayout GetVkImageLayout(ImageLayout layout)
    {
        return vkImageLayoutLUT[static_cast<uint32_t>(layout)];
    }
    static inline VkFormat GetVkImageFormat(ImageFormat format)
    {
        return vkImageFormatLUT[static_cast<uint32_t>(format)];
    }
}