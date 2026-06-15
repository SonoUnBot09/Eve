#pragma once

#include <cstdint>
#include <Type.hpp>

struct Entity
{
    public:
        uint32_t Id;
        uint32_t GenerataionId;
};

struct EntityRecord
{
    public:
        Type Archtype;
        uint32_t BatchIndex;
        uint32_t RowIndex;
};