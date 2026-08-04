#pragma once

#include <vulkan/vulkan.hpp>

#include <Eve/graphics/Buffer.hpp>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/ShaderStages.hpp>
#include <Eve/graphics/Geometry.hpp>

namespace Eve::Graphics
{

    inline static constexpr uint32_t bufferUsageConfigCount = 5;
    inline static constexpr uint32_t imageUsageConfigCount = 6;
    inline static constexpr uint32_t imageAspectMaskConfigCount = 3;
    inline static constexpr uint32_t shaderStageConfigCount = 3;

    // Buffers
    static constexpr VkBufferUsageFlags bufferUsageLUT[]
    {
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
    };

    // Images
    static constexpr VkImageType imageTypeLUT[]
    {
        VK_IMAGE_TYPE_1D,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TYPE_3D
    };

    static constexpr VkImageViewType imageViewTypeLUT[]
    {
        VK_IMAGE_VIEW_TYPE_1D,
        VK_IMAGE_VIEW_TYPE_2D,
        VK_IMAGE_VIEW_TYPE_3D,
        VK_IMAGE_VIEW_TYPE_CUBE,
        VK_IMAGE_VIEW_TYPE_1D_ARRAY,
        VK_IMAGE_VIEW_TYPE_2D_ARRAY,
        VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
    };

    static constexpr VkImageUsageFlags imageUsageLUT[]
    {
        VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_STORAGE_BIT,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT
    };

    static constexpr VkImageLayout imageLayoutLUT[]
    {
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_PREINITIALIZED
    };

    static constexpr VkFormat imageFormatLUT[]
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

    static constexpr uint32_t imageFormatSizeLUT[]
    {
        0,
        1,
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        2,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        4,
        2,
        2,
        2,
        2,
        2,
        4,
        4,
        4,
        4,
        4,
        6,
        6,
        6,
        6,
        6,
        8,
        8,
        8,
        8,
        8,
        4,
        4,
        4,
        8,
        8,
        8,
        12,
        12,
        12,
        16,
        16,
        16,
        2,
        4,
        1,
        4,
        4,
        8
    };

    static constexpr VkSampleCountFlagBits imageSampleCountLUT[]
    {
        VK_SAMPLE_COUNT_1_BIT,
        VK_SAMPLE_COUNT_2_BIT,
        VK_SAMPLE_COUNT_4_BIT,
        VK_SAMPLE_COUNT_8_BIT
    };

    static constexpr VkImageAspectFlags imageAspectMaskLUT[]
    {
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_IMAGE_ASPECT_STENCIL_BIT
    };

    // Sampler
    static constexpr VkFilter samplerFilterModeLUT[]
    {
        VK_FILTER_NEAREST,
        VK_FILTER_LINEAR
    };

    static constexpr VkSamplerMipmapMode samplerMipmapModeLUT[]
    {
        VK_SAMPLER_MIPMAP_MODE_NEAREST,
        VK_SAMPLER_MIPMAP_MODE_LINEAR
    };

    // Shaders Stage
    static constexpr VkShaderStageFlags shaderStageLUT[]
    {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        VK_SHADER_STAGE_COMPUTE_BIT
    };

    // Geometry
    static constexpr VkPrimitiveTopology topologyLUT[]
    {
        VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    static constexpr VkPolygonMode polygonModeLUT[]
    {
        VK_POLYGON_MODE_FILL,
        VK_POLYGON_MODE_LINE,
        VK_POLYGON_MODE_POINT
    };

    static constexpr VkCullModeFlags cullModeLUT[]
    {
        VK_CULL_MODE_NONE,
        VK_CULL_MODE_FRONT_BIT,
        VK_CULL_MODE_BACK_BIT,
        VK_CULL_MODE_FRONT_AND_BACK
    };

    static constexpr VkCompareOp compareOpLUT[]
    {
        VK_COMPARE_OP_ALWAYS,
        VK_COMPARE_OP_LESS,
        VK_COMPARE_OP_LESS_OR_EQUAL,
        VK_COMPARE_OP_GREATER,
        VK_COMPARE_OP_GREATER_OR_EQUAL
    };

    // Buffers
    static inline VkBufferUsageFlags GetVkBufferUsage(BufferUsage usage)
    {
        uint32_t bits = static_cast<uint32_t>(usage);

        VkBufferUsageFlags bufferUsage = 0;
        for(uint32_t i = 0; i < bufferUsageConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            bufferUsage |= bufferUsageLUT[i];
        }

        return bufferUsage;
    }

    // Images
    static inline VkImageType GetVkImageType(TextureType type)
    {
        return imageTypeLUT[static_cast<uint32_t>(type)];
    }
    static inline VkImageViewType GetVkImageViewType(TextureViewType type)
    {
        return imageViewTypeLUT[static_cast<uint32_t>(type)];
    }
    static inline VkImageUsageFlags GetVkImageUsage(TextureUsage usage)
    {
        uint32_t bits = static_cast<uint32_t>(usage);

        VkImageUsageFlags imageUsageFlag = 0;
        for(uint32_t i = 0; i < imageUsageConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            imageUsageFlag |= imageUsageLUT[i];
        }

        return imageUsageFlag;
    }
    static inline VkImageLayout GetVkImageLayout(TextureLayout layout)
    {
        return imageLayoutLUT[static_cast<uint32_t>(layout)];
    }
    static inline VkFormat GetVkImageFormat(Format format)
    {
        return imageFormatLUT[static_cast<uint32_t>(format)];
    }
    static inline VkSampleCountFlagBits GetVkImageSamplesCount(TextureSample sample)
    {
        return imageSampleCountLUT[static_cast<uint32_t>(sample)];
    }
    static inline VkImageAspectFlags GetVkImageAspectMask(TextureAspectMask usage)
    {
        uint32_t bits = static_cast<uint32_t>(usage);

        VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_NONE;
        for(uint32_t i = 0; i < imageAspectMaskConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            aspectMask |= imageAspectMaskLUT[i];
        }

        return aspectMask;
    }

    // Sampler
    static inline VkFilter GetVkFilterMode(Filter filter)
    {
        return samplerFilterModeLUT[static_cast<uint32_t>(filter)];
    }
    static inline VkSamplerMipmapMode GetVkMipmapMode(MipmapMode mipmapMode)
    {
        return samplerMipmapModeLUT[static_cast<uint32_t>(mipmapMode)];
    }

    // Shader stages
    static inline VkShaderStageFlags GetVkShaderStage(ShaderStage stage)
    {
        int32_t bits = static_cast<uint32_t>(stage);

        VkShaderStageFlags shaderStageFlag = 0;
        for(uint32_t i = 0; i < shaderStageConfigCount; i++)
        {
            if(!(bits & (1u << i))) { continue; }

            shaderStageFlag |= shaderStageLUT[i];
        }

        return shaderStageFlag;
    }

    // Geometry
    static inline VkPrimitiveTopology GetVkTopology(Topology topology)
    {
        return topologyLUT[static_cast<uint32_t>(topology)];
    }
    static inline VkPolygonMode GetVkPolygonMode(PolygonMode polygonMode)
    {
        return polygonModeLUT[static_cast<uint32_t>(polygonMode)];
    }
    static inline VkCullModeFlags GetVkCullMode(CullMode cullMode)
    {
        return cullModeLUT[static_cast<uint32_t>(cullMode)];
    }
    static inline VkCompareOp GetVkCompareOp(DepthTest compareOp)
    {
        return compareOpLUT[static_cast<uint32_t>(compareOp)];
    }

    static inline VkImageAspectFlags GetVkImageAspectMaskBasedOnFormat(VkFormat format)
    {
        switch (format)
        {
            case (VK_FORMAT_D16_UNORM) :
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            case (VK_FORMAT_D32_SFLOAT) :
                return VK_IMAGE_ASPECT_DEPTH_BIT;

            case (VK_FORMAT_S8_UINT) :
                return VK_IMAGE_ASPECT_STENCIL_BIT;

            case (VK_FORMAT_D16_UNORM_S8_UINT) :
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            case (VK_FORMAT_D24_UNORM_S8_UINT) :
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            case (VK_FORMAT_D32_SFLOAT_S8_UINT) :
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

            default: return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }
}