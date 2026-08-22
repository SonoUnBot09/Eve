#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region 

        enum class Filter
        {
            FILTER_NEAREST,
            FILTER_LINEAR
        };

        enum class MipmapMode
        {
            MIPMAP_NEAREST,
            MIPMAP_LINEAR
        };

    #pragma endregion

    struct SamplerHandle
    {
        SamplerHandle() = default;
        uint32_t Id;
        uint32_t Generation;
    };

    struct SamplerInfo
    {
        Filter MinFilter;
        Filter MagFilter;
        MipmapMode MipmapMode;
    };

}