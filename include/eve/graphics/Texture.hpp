#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region

        enum class TextureType : uint16_t
        {
            TEXTURE_1D,
            TEXTURE_2D,
            TEXTURE_3D,
            TEXTURE_CUBE
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

        enum class TextureAspectMask : uint16_t
        {
            ASPECT_MASK_COLOR = 1 << 0,
            ASPECT_MASK_DEPTH = 1 << 1,
            ASPECT_MASK_STENCIL = 1 << 2
        };

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
        TextureHandle(uint32_t id, uint32_t generation) : Id(id), Generation(generation) {}; 
        uint32_t Id;
        uint32_t Generation;
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
        Format Format;
        bool randomReadWrite;
    };

    struct TextureInfo2D
    {
        uint32_t Width, Height;
        Format Format;
        bool randomReadWrite;
    };

    struct TextureInfo3D
    {
        uint32_t Height, Width, Depth;
        Format Format;
        bool randomReadWrite;
    };

    struct TransientTextureInfo1D
    {
        uint32_t Width;

        Format Format;
    };

    struct TransientTextureInfo2D
    {
        uint32_t Width, Height;

        Format Format;
    };

    struct TransientTextureInfo3D
    {
        uint32_t Height, Width, Depth;

        Format Format;
    };
}