#include "Table.hpp"

std::tuple<uint32_t, uint32_t> Table::GetNewEntitySlot()
{   
    uint32_t batchesSize = batches.size();

    if(batches.size() == 0)
    {
        AllocateBatches(1);
        return {0, 0};
    }

    uint32_t lastBatchIndex = batchesSize - 1;

    uint32_t componentsCount = GetComponentsCountPerBatch(lastBatchIndex);

    if(componentsCount == maxSingleComponentPerBatch)
    {
        AllocateBatches(1);
        return { batchesSize, 0};
    }
    else
    {
        return {lastBatchIndex, componentsCount};
    }
}

void Table::AllocateBatches(const uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        std::byte* batchArray = new std::byte[batchSize];
        uint32_t* entitiesIndicesArray = new uint32_t[maxSingleComponentPerBatch];
        std::vector<bool> holesArray(maxSingleComponentPerBatch);

        batches.push_back(batchArray);
        entitiesIndices.push_back(entitiesIndicesArray);
        holesBit.push_back(holesArray);

        componentsCountPerBatch.push_back(0);
    }
}

void Table::DeallocateAllBatches(std::vector<std::byte*> batches)
{
    for (uint32_t i = 0; i < batches.size(); i++)
    {
        delete[] batches[i];
        delete[] entitiesIndices[i];
    }

    batches.clear();
    entitiesIndices.clear();
    componentsCountPerBatch.clear();
}


/*
void Table::DeallocateBatch(const uint32_t index)
{  
    delete[] batches[index];

    batches.erase(batches.cbegin() + index);
}*/