#pragma once

#include <vector>

struct Batch
{
    std::vector<std::byte> BatchData;
    std::vector<std::uint32_t> EntitiesID;
    std::vector<bool> ActiveEntities;
    uint32_t PeakBatchSize;
    uint32_t ActiveEntitiesCount;

    Batch(uint32_t batchSizeBytes, uint32_t maxEntitiesPerBatch) : PeakBatchSize(0), ActiveEntitiesCount(0)
    {
        BatchData.resize(batchSizeBytes);
        EntitiesID.resize(maxEntitiesPerBatch);
        ActiveEntities.resize(maxEntitiesPerBatch);
    }
};