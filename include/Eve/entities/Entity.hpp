#pragma once

#include <cstdint>
#include <eve/entities/Type.hpp>
#include <eve/entities/Table.hpp>

namespace Eve::Entities
{
    struct Entity
    {
        Entity() = default;
        Entity(uint32_t id, uint32_t generationId) 
            : Id(id), GeneratationId(generationId) {}
        public:
            uint32_t Id;
            uint32_t GeneratationId;
    };

    struct EntityRecord
    {
        EntityRecord() = default;
        EntityRecord(Table& table, uint32_t batchIndex, uint32_t rowIndex, Type archtype) :
        Table(&table), BatchIndex(batchIndex), RowIndex(rowIndex), Archtype(archtype) {};

        Table* Table;
        uint32_t BatchIndex;
        uint32_t RowIndex;
        Type Archtype;
    };
}