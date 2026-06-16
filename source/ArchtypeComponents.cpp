#include "ArchtypeComponents.hpp"

char* ArchtypeComponents::GetBatch(const uint32_t index)
{
    uint32_t batchesCount = batches.size();

    if(!(index < batchesCount)) { return nullptr; }

    return batches[index];
}

const MemoryInfo* ArchtypeComponents::GetMemoryInfo(const Type componentType)
{
    return memoryLayout.GetMemoryInfo(componentType);
}

void ArchtypeComponents::AllocateBatches(const uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        char* batchArray = new char[batchSize];

        batches.push_back(batchArray);
    }
}

void ArchtypeComponents::DeallocateAllBatches(std::vector<char*> batches)
{
    for (uint32_t i = 0; i < batches.size(); i++)
    {
        delete[] batches[i];
    }

    batches.clear();
}


/*
void ArchtypeComponents::DeallocateBatch(const uint32_t index)
{  
    delete[] batches[index];

    batches.erase(batches.cbegin() + index);
}*/