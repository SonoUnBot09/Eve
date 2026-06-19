#pragma once

#include <vector>
#include <cstdint>
#include "MemoryInfo.hpp"
#include "Type.hpp"
#include "MemoryLayout.hpp"

class Table
{
    public:

        Table(Type archtype, uint32_t batchSizeInByte) : 
        archtype(archtype), 
        batchSize(batchSizeInByte), 
        memoryLayout(archtype, batchSizeInByte)
        {
            maxSingleComponentPerBatch = memoryLayout.GetMaxSingleComponentsCountPerBatch();
        };

        char* GetBatch(const uint32_t index);
        const uint32_t GetBatchComponentsCount(const uint32_t index);
        const MemoryInfo* GetMemoryInfo(const Type componentType); 

        template<typename T>
        T& GetComponent(const uint32_t index, char* batch, const MemoryInfo& memoryInfo)
        {
            return *reinterpret_cast<T*>(
                batch + memoryInfo.offset + memoryInfo.stride * index
            );
        }
        
        ~Table()
        {
            DeallocateAllBatches(batches);
        }

    private:

        Type archtype;
        uint32_t batchSize;

        uint32_t maxSingleComponentPerBatch;
        MemoryLayout memoryLayout;

        std::vector<uint32_t*> entitisIndices;
        std::vector<char*> batches;
        std::vector<uint32_t> componentsCountPerBatch;

        void AllocateBatches(const uint32_t count);

        void DeallocateAllBatches(const std::vector<char*> batches);
        //void DeallocateBatch(const uint32_t index);
};