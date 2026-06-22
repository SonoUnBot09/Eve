#pragma once

#include <cstdint>
#include <Type.hpp>
#include "Table.hpp"

struct Entity
{
    Entity() = default;
    Entity(uint32_t id, uint32_t generationId) 
        : Id(id), GenerataionId(generationId) {}
    public:
        uint32_t Id;
        uint32_t GenerataionId;
};

struct EntityRecord
{
    EntityRecord() = default;
    EntityRecord(Table* table, uint32_t batchIndex, uint32_t rowIndex, Type archtype) :
    Table(table), BatchIndex(batchIndex), RowIndex(rowIndex), Archtype(archtype) {};

    Table* Table;
    uint32_t BatchIndex;
    uint32_t RowIndex;
    Type Archtype;
};

struct SlotInfo
{
    SlotInfo() = default;
    SlotInfo(uint32_t id, uint32_t generationId, Table* table, uint32_t batchIndex, uint32_t rowIndex, Type archtype, bool isHole) :
    Id(id), GenerationId(generationId), Table(table), BatchIndex(batchIndex), RowIndex(rowIndex), Archtype(archtype), IsHole(isHole) {};

    uint32_t Id;
    uint32_t GenerationId;
    Table* Table;
    uint32_t BatchIndex;
    uint32_t RowIndex;
    Type Archtype;
    bool IsHole;
};