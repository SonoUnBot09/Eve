#pragma once 

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region

        enum class BufferType : uint16_t
        {
            BUFFER_TYPE_VERTEX       = 1 << 0,
            BUFFER_TYPE_INDEX        = 1 << 1,
            BUFFER_TYPE_UNIFORM      = 1 << 2,
            BUFFER_TYPE_TRANSFER_SRC = 1 << 3,
            BUFFER_TYPE_TRANSFER_DST = 1 << 4,
            BUFFER_TYPE_STORAGE = 1 << 5
        };

        inline BufferType operator|(BufferType a, BufferType b)
        {
            return static_cast<BufferType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline BufferType operator& (BufferType a, BufferType b)
        {
            return static_cast<BufferType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }

    #pragma endregion

    struct BufferHandle
    {
        BufferHandle() = default;
        BufferHandle(uint32_t id, uint32_t generationId) : Id(id) {}; 
        uint32_t Id;
    };

    struct BufferInfo
    {
        uint64_t Size;
        BufferType Type;
    };

}