#pragma once

#include <cstdint>

namespace Eve::Entities
{
    struct MemoryInfo
    {
        uint32_t stride;
        uint32_t offset;

        MemoryInfo() = default;
        MemoryInfo(uint32_t stride, uint32_t offset) : stride(stride), offset(offset) {};
    };
}