#pragma once

#include <cstdint>

namespace Eve::Entities
{
    class Table;

    struct SlotInfo
    {
        SlotInfo(uint32_t batchIndex, uint32_t rowIndex) :
        BatchIndex(batchIndex), RowIndex(rowIndex) {};

        uint32_t BatchIndex;
        uint32_t RowIndex;
    };
}