#pragma once

#include <vector>
#include <cstdint>
#include <memory>

#include <eve/entities/MemoryInfo.hpp>
#include <eve/entities/Type.hpp>
#include <entities/MemoryLayout.hpp>
#include <entities/Batch.hpp>
#include <eve/entities/SlotInfo.hpp>

namespace Eve::Entities
{
    class EntityManagerA;

    class Table
    {
        public:

            Table(Type archtype, uint32_t batchSizeByte);

            void CreateBatch();
            void DestroyLastBatch();


            void FreeSlot(SlotInfo slotInfo);
            SlotInfo GetNewSlot(uint32_t entityId);

            void* GetComponentPtr(SlotInfo slotInfo, Type componentType);
            void WriteComponents(SlotInfo slotInfo, std::vector<Type>& componentTypes, std::vector<void*>& sources);

            void CompactBatches();

        private:

            SlotInfo FindValidEntity(int32_t batchIndex, int32_t rowIndex);
            void WriteComponents(SlotInfo srcSlot, SlotInfo dstSlot);

            Type archtype;
            uint32_t batchSizeBytes;

            uint32_t maxEntitiesPerBatch;
            MemoryLayout memoryLayout;

            std::vector<Batch> batches;
            uint32_t entitiesCount;

            std::vector<SlotInfo> freeSlots;
           

            friend class EntityManager;
    };
}