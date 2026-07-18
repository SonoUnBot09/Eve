#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region

        enum class TextureType : uint16_t
        {
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D  
        };

        enum class TextureViewType : uint16_t
        {
            IMAGE_VIEW_TYPE_1D,
            IMAGE_VIEW_TYPE_2D,
            IMAGE_VIEW_TYPE_3D ,
            IMAGE_VIEW_TYPE_CUBE,
            IMAGE_VIEW_TYPE_1D_ARRAY,
            IMAGE_VIEW_TYPE_2D_ARRAY,
            IMAGE_VIEW_TYPE_CUBE_ARRAY
        };

        enum class TextureUsage : uint16_t
        {
            USAGE_SAMPLED                   = 1 << 0,
            USAGE_STORAGE                   = 1 << 1,
            USAGE_COLOR_ATTACHMENT          = 1 << 2,
            USAGE_DEPTH_STENCIL_ATTACHMENT  = 1 << 3,
            USAGE_TRANSFER_SRC              = 1 << 4,
            USAGE_TRANSFER_DST              = 1 << 5,
            USAGE_TRANSIENT_ATTACHMENT      = 1 << 6
        };

        enum class TextureLayout : uint16_t
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

        enum class Format : uint16_t
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

        enum class TextureSample : uint16_t
        {
            SAMPLE_1 = 1,
            SAMPLE_2 = 2,
            SAMPLE_4 = 4,
            SAMPLE_8 = 8
        };

        enum class TextureAspectMask : uint16_t
        {
            ASPECT_MASK_COLOR = 1 << 0,
            ASPECT_MASK_DEPTH = 1 << 1,
            ASPECT_MASK_STENCIL = 1 << 2
        };

        inline TextureUsage operator|(TextureUsage a, TextureUsage b)
        {
            return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline TextureUsage operator&(TextureUsage a, TextureUsage b)
        {
            return static_cast<TextureUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }
        inline TextureUsage& operator|=(TextureUsage& a, TextureUsage b)
        {
            a = a | b;
            return a;
        }
        inline TextureUsage& operator&=(TextureUsage& a, TextureUsage b)
        {
            a = a & b;
            return a;
        }

        inline TextureAspectMask operator|(TextureAspectMask a, TextureAspectMask b)
        {
            return static_cast<TextureAspectMask>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline TextureAspectMask operator&(TextureAspectMask a, TextureAspectMask b)
        {
            return static_cast<TextureAspectMask>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }
        inline TextureAspectMask& operator|=(TextureAspectMask& a, TextureAspectMask b)
        {
            a = a | b;
            return a;
        }
        inline TextureAspectMask& operator&=(TextureAspectMask& a, TextureAspectMask b)
        {
            a = a & b;
            return a;
        }

    #pragma endregion

    struct TextureHandle
    {
        TextureHandle() = default;
        TextureHandle(uint32_t id) : Id(id) {}; 
        uint32_t Id;
    };

    struct TransientTextureHandle
    {
        TransientTextureHandle() = default;
        TransientTextureHandle(uint32_t id) : Id(id) {}; 
        uint32_t Id;
    };

    struct TextureInfo1D
    {
        uint32_t Width;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureUsage Usage;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    struct TextureInfo2D
    {
        uint32_t Width, Height;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureUsage Usage;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    struct TextureInfo3D
    {
        uint32_t Height, Width, Depth;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureUsage Usage;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    struct TransientTextureInfo1D
    {
        uint32_t Width;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    struct TransientTextureInfo2D
    {
        uint32_t Width, Height;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    struct TransientTextureInfo3D
    {
        uint32_t Height, Width, Depth;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
        TextureSample Sample = TextureSample::SAMPLE_1;
    };

    #pragma pack(push, 1)
    struct TextureInfo
    {
        uint32_t Width, Height, Depth;
        uint32_t ArrayLayers;
        uint32_t MipLevels;

        Format Format;
        TextureUsage Usage;
        TextureSample Sample;
    };
    #pragma pack(pop)

    /*
    struct TransientTextureInfo2D
    {
        uint32_t Height, Width;
        uint32_t ArrayLayers = 1;
        uint32_t MipLevels = 1;

        Format Format;
    }*/
}