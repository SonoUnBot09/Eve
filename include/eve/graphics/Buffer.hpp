#pragma once 

#include <cstdint>

namespace Eve::Graphics
{
    #pragma region

        enum class BufferUsage : uint16_t
        {
            BUFFER_USAGE_INDEX        = 1 << 0,
            BUFFER_USAGE_UNIFORM      = 1 << 1,
            BUFFER_USAGE_TRANSFER_SRC = 1 << 2,
            BUFFER_USAGE_TRANSFER_DST = 1 << 3,
            BUFFER_USAGE_STORAGE = 1 << 4
        };

        inline BufferUsage operator|(BufferUsage a, BufferUsage b)
        {
            return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline BufferUsage operator& (BufferUsage a, BufferUsage b)
        {
            return static_cast<BufferUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }
        inline BufferUsage& operator|=(BufferUsage& a, BufferUsage b)
        {
            a = a | b;
            return a;
        }
        inline BufferUsage& operator&=(BufferUsage& a, BufferUsage b)
        {
            a = a & b;
            return a;
        }

    #pragma endregion

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

    struct BufferInfo
    {
        uint64_t Size;
        BufferUsage Usage = static_cast<BufferUsage>(0);

        bool operator==(const BufferInfo& other) const
        {
            return 
                Size == other.Size &&
                Usage == other.Usage;
        }
    };

    struct TransientBufferInfo
    {
        uint64_t Size;
    };

}