#include "Table.hpp"

char* Table::GetBatch(const uint32_t index)
{
    return batches[index];
}
const uint32_t Table::GetBatchComponentsCount(const uint32_t index)
{
    return componentsCountPerBatch[index];
}

const MemoryInfo* Table::GetMemoryInfo(const Type componentType)
{
    return memoryLayout.GetMemoryInfo(componentType);
}

void Table::AllocateBatches(const uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        char* batchArray = new char[batchSize];
        uint32_t* entitiesIndicesArray = new uint32_t[maxSingleComponentPerBatch];

        batches.push_back(batchArray);
        entitisIndices.push_back(entitiesIndicesArray);

        componentsCountPerBatch.push_back(0);
    }
}

void Table::DeallocateAllBatches(std::vector<char*> batches)
{
    for (uint32_t i = 0; i < batches.size(); i++)
    {
        delete[] batches[i];
        delete[] entitisIndices[i];
    }

    batches.clear();
    entitisIndices.clear();
    componentsCountPerBatch.clear();
}


/*
void Table::DeallocateBatch(const uint32_t index)
{  
    delete[] batches[index];

    batches.erase(batches.cbegin() + index);
}*/