#pragma once

#include <cstdint>
#include <Eve/Entities/Table.hpp>

struct SlotInfo
{
    SlotInfo() = default;
    SlotInfo(uint32_t id, uint32_t generationId, Eve::Entities::Table* table, uint32_t batchIndex, uint32_t rowIndex, Type archtype, bool isHole) :
    Id(id), GenerationId(generationId), Table(table), BatchIndex(batchIndex), RowIndex(rowIndex), Archtype(archtype), IsHole(isHole) {};

    uint32_t Id;
    uint32_t GenerationId;
    Eve::Entities::Table* Table;
    uint32_t BatchIndex;
    uint32_t RowIndex;
    Type Archtype;
    bool IsHole;
};