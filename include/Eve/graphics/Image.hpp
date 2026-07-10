#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region

        enum class ImageType : uint16_t
        {
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D  
        };

        enum class ImageViewType : uint16_t
        {
            IMAGE_VIEW_TYPE_1D,
            IMAGE_VIEW_TYPE_2D,
            IMAGE_VIEW_TYPE_3D ,
            IMAGE_VIEW_TYPE_CUBE,
            IMAGE_VIEW_TYPE_1D_ARRAY,
            IMAGE_VIEW_TYPE_2D_ARRAY,
            IMAGE_VIEW_TYPE_CUBE_ARRAY
        };

        enum class ImageUsage : uint16_t
        {
            USAGE_SAMPLED                   = 1 << 0,
            USAGE_STORAGE                   = 1 << 1,
            USAGE_COLOR_ATTACHMENT          = 1 << 2,
            USAGE_DEPTH_STENCIL_ATTACHMENT  = 1 << 3,
            USAGE_TRANSFER_SRC              = 1 << 4,
            USAGE_TRANSFER_DST              = 1 << 5,
            USAGE_TRANSIENT_ATTACHMENT      = 1 << 6
        };

        enum class ImageLayout : uint16_t
        {
            LAYOUT_UNDEFINED,
            LAYOUT_GENERAL,
            LAYOUT_COLOR_ATTACHMENT,
            LAYOUT_DEPTH_STENCIL_ATTACHMENT,
            LAYOUT_DEPTH_ATTACHMENT,
            LAYOUT_STENCIL_ATTACHMENT,
            LAYOUT_SHADER_READ_ONLY,
            LAYOUT_TRANSFER_SRC,
            LAYOUT_TRANSFER_DST,
            LAYOUT_PRESENT,
            LAYOUT_PREINITIALIZED
        };

        enum class ImageFormat : uint16_t
        {
            FORMAT_UNDEFINED = 0,
            FORMAT_R8_UNORM,
            FORMAT_R8_SNORM,
            FORMAT_R8_UINT,
            FORMAT_R8_SINT,
            FORMAT_R8_SRGB,
            FORMAT_R8G8_UNORM,
            FORMAT_R8G8_SNORM,
            FORMAT_R8G8_UINT,
            FORMAT_R8G8_SINT,
            FORMAT_R8G8_SRGB,
            FORMAT_R8G8B8_UNORM,
            FORMAT_R8G8B8_SNORM,
            FORMAT_R8G8B8_UINT,
            FORMAT_R8G8B8_SINT,
            FORMAT_R8G8B8_SRGB,
            FORMAT_B8G8R8_UNORM,
            FORMAT_B8G8R8_SNORM,
            FORMAT_B8G8R8_UINT,
            FORMAT_B8G8R8_SINT,
            FORMAT_B8G8R8_SRGB,
            FORMAT_R8G8B8A8_UNORM,
            FORMAT_R8G8B8A8_SNORM,
            FORMAT_R8G8B8A8_UINT,
            FORMAT_R8G8B8A8_SINT,
            FORMAT_R8G8B8A8_SRGB,
            FORMAT_B8G8R8A8_UNORM,
            FORMAT_B8G8R8A8_SNORM,
            FORMAT_B8G8R8A8_UINT,
            FORMAT_B8G8R8A8_SINT,
            FORMAT_B8G8R8A8_SRGB,
            FORMAT_R16_UNORM,
            FORMAT_R16_SNORM,
            FORMAT_R16_UINT,
            FORMAT_R16_SINT,
            FORMAT_R16_SFLOAT,
            FORMAT_R16G16_UNORM,
            FORMAT_R16G16_SNORM,
            FORMAT_R16G16_UINT,
            FORMAT_R16G16_SINT,
            FORMAT_R16G16_SFLOAT,
            FORMAT_R16G16B16_UNORM,
            FORMAT_R16G16B16_SNORM,
            FORMAT_R16G16B16_UINT,
            FORMAT_R16G16B16_SINT,
            FORMAT_R16G16B16_SFLOAT,
            FORMAT_R16G16B16A16_UNORM,
            FORMAT_R16G16B16A16_SNORM,
            FORMAT_R16G16B16A16_UINT,
            FORMAT_R16G16B16A16_SINT,
            FORMAT_R16G16B16A16_SFLOAT,
            FORMAT_R32_UINT,
            FORMAT_R32_SINT,
            FORMAT_R32_SFLOAT,
            FORMAT_R32G32_UINT,
            FORMAT_R32G32_SINT,
            FORMAT_R32G32_SFLOAT,
            FORMAT_R32G32B32_UINT,
            FORMAT_R32G32B32_SINT,
            FORMAT_R32G32B32_SFLOAT,
            FORMAT_R32G32B32A32_UINT,
            FORMAT_R32G32B32A32_SINT,
            FORMAT_R32G32B32A32_SFLOAT,
            FORMAT_D16_UNORM,
            FORMAT_D32_SFLOAT,
            FORMAT_S8_UINT,
            FORMAT_D16_UNORM_S8_UINT,
            FORMAT_D24_UNORM_S8_UINT,
            FORMAT_D32_SFLOAT_S8_UINT
        };

        enum class ImageSample : uint16_t
        {
            SAMPLE_1 = 1,
            SAMPLE_2 = 2,
            SAMPLE_4 = 4,
            SAMPLE_8 = 8
        };

        inline ImageUsage operator|(ImageUsage a, ImageUsage b)
        {
            return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline ImageUsage operator&(ImageUsage a, ImageUsage b)
        {
            return static_cast<ImageUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }

    #pragma endregion

    struct ImageHandle
    {
        ImageHandle(uint32_t id, uint32_t generationId) : Id(id), GenerationId(generationId) {}; 
        uint32_t Id;
        uint32_t GenerationId;
    };

    struct ImageInfo
    {
        uint32_t Height, Width, Depth;
        uint32_t ArrayLayers;
        uint32_t MipLevels;

        ImageType Type;
        ImageViewType ViewType;
        ImageUsage Usage;
        ImageLayout Layuot;
        ImageFormat Format;
        ImageSample SampleCount;
    };
}