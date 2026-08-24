#pragma once 

#include <cstdint>

namespace Eve::Graphics
{
    struct BufferHandle
    {
        BufferHandle() = default;
        BufferHandle(uint32_t id, uint32_t generation) : Id(id), Generation(generation) {}; 
        uint32_t Id;
        uint32_t Generation;
    };

    struct TransientBufferHandle
    {
        TransientBufferHandle() = default;
        TransientBufferHandle(uint32_t id) : Id(id) {}; 
        uint32_t Id;
    };
}