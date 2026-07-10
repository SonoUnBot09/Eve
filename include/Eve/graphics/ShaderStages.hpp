#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    enum class ShaderStage : uint16_t
    {
        VERTEX_STAGE = 1 << 0,
        FRAGMENT_STAGE = 1 << 1,
        COMPUTE_STAGE = 1 << 2
    };
}