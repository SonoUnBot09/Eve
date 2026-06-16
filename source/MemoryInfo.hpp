#pragma once
#include <cstdint>

struct MemoryInfo
{
    uint32_t stride;
    uint32_t offset;

    MemoryInfo(uint32_t stride, uint32_t offset) : stride(stride), offset(offset) {};
};