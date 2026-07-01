#pragma once

#include <vector>
#include <cstdint>

#include <Eve/MemoryInfo.hpp>
#include <Eve/Type.hpp>
#include <Eve/internal/MemoryLayout.hpp>

namespace Eve::Entities
{
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


            inline std::byte* GetComponentsBatch(const uint32_t index) { return batches[index]; }
            inline const MemoryInfo* GetMemoryInfo(const Type componentType) { return memoryLayout.GetMemoryInfo(componentType); }

            inline uint32_t GetBatchesCount() { return batches.size(); }
            inline uint32_t GetComponentsCountPerBatch(const uint32_t batchIndex) { return componentsCountPerBatch[batchIndex]; }
            inline int32_t GetLastBatchIndex() { return batches.size() - 1; }


            template<typename T>
            inline T& GetComponent(std::byte* batch, const uint32_t rowIndex, const MemoryInfo& memoryInfo)
            { return *reinterpret_cast<T*>(batch + memoryInfo.offset + memoryInfo.stride * rowIndex); }

            ~Table()
            {
                DeallocateAllBatches();
            }

        private:

            Type archtype;
            uint32_t batchSize;

            uint32_t maxSingleComponentPerBatch;
            Eve::Internal::MemoryLayout memoryLayout;

            std::vector<uint32_t*> entitiesIndices;
            std::vector<std::vector<bool>> holesBit;
            std::vector<std::byte*> batches;
            std::vector<uint32_t> componentsCountPerBatch;

            std::tuple<uint32_t, uint32_t> GetNewEntitySlot();

            inline uint32_t GetMaxComponentsCountPerBatch() { return maxSingleComponentPerBatch; }

            inline void SetComponentsCountPerBatch(const uint32_t batchIndex, const uint32_t value) {componentsCountPerBatch[batchIndex] = value; }

            inline void SetEntityIndex(const uint32_t batchIndex, const uint32_t rowIndex, const uint32_t value) { entitiesIndices[batchIndex][rowIndex] = value; }
            inline void SetEntityHolesBitIndex(const uint32_t batchIndex, const uint32_t rowIndex, const bool isHole) { holesBit[batchIndex][rowIndex] = isHole; }

            void AllocateBatches(const uint32_t count);
            void DeallocateAllBatches();
            void DeallocateBatch(const uint32_t index);

            friend class EntityManager;
    };
}