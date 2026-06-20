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
    EntityRecord(Type archtype, uint32_t batchIndex, uint32_t rowIndex) :
    Archtype(archtype), BatchIndex(batchIndex), RowIndex(rowIndex) {};
    public:
        Type Archtype;
        uint32_t BatchIndex;
        uint32_t RowIndex;
};

struct EntityInfo
{
    EntityInfo() = default;
    EntityInfo(uint32_t id, Type archtype, uint32_t batchIndex, uint32_t rowIndex, bool valid) :
    Archtype(archtype), BatchIndex(batchIndex), RowIndex(rowIndex), Id(id), Valid(valid) {};
    public:
        Type Archtype;
        uint32_t BatchIndex;
        uint32_t RowIndex;
        uint32_t Id;
        bool Valid;
};