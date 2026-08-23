#pragma once

#include <vector>
#include <cstdint>

#include <eve/entities/Type.hpp>
#include <eve/entities/details/MemoryLayout.hpp>
#include <eve/entities/details/Batch.hpp>
#include <eve/entities/details/SlotInfo.hpp>

#include <iostream>

namespace Eve::Entities
{
    class Table
    {
        public:

            Table(Type archtype, uint32_t batchSizeByte);

            uint32_t GetEntitiesCount();

            // TODO : Get Components Feature
            template<typename T>
            T& GetComponent(uint32_t index, Type componentType)
            {
                uint32_t batchIndex = std::floor((float)index / (float)maxEntitiesPerBatch);
                uint32_t localIndex = index - (batchIndex * maxEntitiesPerBatch);

                std::cout << "GetComponent: " << batchIndex << "  " << localIndex << std::endl;

                return *reinterpret_cast<T*>
                (
                    GetComponentPtr({batchIndex, localIndex}, componentType)
                );
            }

        private:

            void CreateBatch();
            void DestroyLastBatch();

            void FreeSlot(SlotInfo slotInfo);
            SlotInfo GetNewSlot(uint32_t entityId);

            SlotInfo FindValidEntity(int32_t batchIndex, int32_t rowIndex);
            void WriteComponents(SlotInfo srcSlot, SlotInfo dstSlot);

            void* GetComponentPtr(SlotInfo slotInfo, Type componentType);
            void WriteComponents(SlotInfo slotInfo, std::vector<Type>& componentTypes, std::vector<void*>& sources);

            void CompactBatches();

            Type archtype;
            uint32_t batchSizeBytes;

            uint32_t maxEntitiesPerBatch;
            MemoryLayout memoryLayout;

            std::vector<Batch> batches;
            uint32_t batchesCount;
            uint32_t entitiesCount;

            std::vector<SlotInfo> freeSlots;
           
            friend class EntityManager;
    };
}