#pragma once

#include <vector>
#include <cstdint>
#include "MemoryInfo.hpp"
#include "Type.hpp"
#include "MemoryLayout.hpp"

class ArchtypeComponents
{
    public:

        ArchtypeComponents(Type archtype, uint32_t batchSizeInByte) : 
        archtype(archtype), 
        batchSize(batchSizeInByte), 
        memoryLayout(archtype, batchSizeInByte) {};

        char* GetBatch(const uint32_t index);
        const MemoryInfo* GetMemoryInfo(const Type componentType); 

        template<typename T>
        T& GetComponent(const uint32_t index, char* batch, const MemoryInfo& memoryInfo)
        {
            return *reinterpret_cast<T*>(
                batch + memoryInfo.offset + memoryInfo.stride * index
            );
        }
        
        ~ArchtypeComponents()
        {
            DeallocateAllBatches(batches);
        }

    private:

        Type archtype;
        uint32_t batchSize;

        MemoryLayout memoryLayout;

        std::vector<char*> batches;

        void AllocateBatches(const uint32_t count);

        void DeallocateAllBatches(const std::vector<char*> batches);
        //void DeallocateBatch(const uint32_t index);
};