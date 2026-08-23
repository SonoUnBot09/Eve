#pragma once

#include <vector>
#include <cstdint>

#include <eve/entities/Type.hpp>
#include <eve/entities/Entity.hpp>
#include <eve/entities/details/MemoryLayout.hpp>
#include <eve/entities/details/Batch.hpp>
#include <eve/entities/details/SlotInfo.hpp>

namespace Eve::Entities
{
    class Table
    {
        public:

            Table(Type archtype, uint32_t batchSizeByte);

            uint32_t GetEntitiesCount();
            SlotInfo GetSlotInfo(uint32_t index);

            template<typename T>
            T& GetComponent(uint32_t index, Type componentType)
            {
                SlotInfo slotInfo = GetSlotInfo(index);

                return *reinterpret_cast<T*>
                (
                    GetComponentPtr(slotInfo, componentType)
                );
            }
            template<typename T>
            T& GetComponent(SlotInfo slotInfo, Type componentType)
            {
                return *reinterpret_cast<T*>
                (
                    GetComponentPtr(slotInfo, componentType)
                );
            }

            Entity GetEntity(uint32_t index);
            Entity GetEntity(SlotInfo slotInfo);

        private:

            void CreateBatch();
            void DestroyLastBatch();

            void FreeSlot(SlotInfo slotInfo);
            SlotInfo GetNewSlot(uint32_t entityId);

            SlotInfo FindFreeSlot(uint32_t batchIndex, uint32_t rowIndex);
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