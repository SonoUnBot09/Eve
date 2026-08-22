#pragma once

#include <cstdint>

namespace Eve::Entities
{
    struct SlotInfo
    {
        SlotInfo() = default;
        SlotInfo(uint32_t batchIndex, uint32_t rowIndex) :
        BatchIndex(batchIndex), RowIndex(rowIndex) {};

        uint32_t BatchIndex;
        uint32_t RowIndex;
    };
}