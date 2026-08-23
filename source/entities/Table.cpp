#include "eve/entities/details/SlotInfo.hpp"
#include <eve/entities/ComponentsRegistry.hpp>
#include <eve/entities/Table.hpp>
#include <eve/entities/EntityManager.hpp>

Table::Table(Type archtype, uint32_t batchSizeBytes) :
archtype(archtype), 
batchSizeBytes(batchSizeBytes),
memoryLayout(MemoryLayout(archtype, batchSizeBytes)),
batchesCount(0), entitiesCount(0)
{
    // TODO: Check if the batch size is enough
    maxEntitiesPerBatch = memoryLayout.GetMaxEntityCountPerBatch();
}

void Table::CreateBatch()
{
    batches.emplace_back(batchSizeBytes, maxEntitiesPerBatch);
    batchesCount++;
}

void Table::DestroyLastBatch()
{
    batches.pop_back();
    batchesCount--;
}

void Table::FreeSlot(SlotInfo slotInfo)
{
    entitiesCount--;

    freeSlots.push_back(slotInfo);

    Batch& batch = batches[slotInfo.BatchIndex];

    batch.ActiveEntities[slotInfo.RowIndex] = false;
    batch.ActiveEntitiesCount--;
}

SlotInfo Table::GetNewSlot(uint32_t entityId)
{
    entitiesCount++;

    SlotInfo slotInfo;

    if(batches.empty())
    {
        CreateBatch();
    }

    if(freeSlots.empty())
    {
        uint32_t lastBatchIndex = batches.size() - 1;

        Batch& batch = batches[lastBatchIndex];
        if(batch.PeakBatchSize == maxEntitiesPerBatch)
        {
            CreateBatch();
            slotInfo.BatchIndex = lastBatchIndex + 1;
            slotInfo.RowIndex = 0;
        }
        else 
        {
            slotInfo.BatchIndex = lastBatchIndex;
            slotInfo.RowIndex = batch.PeakBatchSize;
        }
    }
    else 
    {
        slotInfo = freeSlots.back();
        freeSlots.pop_back();
    }   

    Batch& batch = batches[slotInfo.BatchIndex];

    batch.PeakBatchSize++;
    batch.ActiveEntities[slotInfo.RowIndex] = true;
    batch.EntitiesID[slotInfo.RowIndex] = entityId;
    batch.ActiveEntitiesCount++;

    return slotInfo;
}

void* Table::GetComponentPtr(SlotInfo slotInfo, Type componentType)
{
    MemoryInfo memoryInfo = memoryLayout.GetMemoryInfo(componentType);

    std::byte* dstBatch = batches[slotInfo.BatchIndex].BatchData.data();

    return dstBatch + memoryInfo.offset + slotInfo.RowIndex * memoryInfo.stride;
}

void Table::WriteComponents(SlotInfo slotInfo, std::vector<Type>& componentTypes, std::vector<void*>& sources)
{
    uint32_t componentsCount = componentTypes.size();

    Batch& batch = batches[slotInfo.BatchIndex];
    std::byte* dstBatch = batch.BatchData.data();

    for(uint32_t i = 0; i < componentsCount; i++)
    {
        Type componentType = componentTypes[i];

        size_t size = ComponentsRegistry::GetComponentSize(componentType);

        MemoryInfo memoryInfo = memoryLayout.GetMemoryInfo(componentType);

        memcpy
        (
            GetComponentPtr(slotInfo, componentType),
            sources[i],
            size
        );

    }
}

void Table::WriteComponents(SlotInfo srcSlot, SlotInfo dstSlot)
{
    std::byte* srcData = batches[srcSlot.BatchIndex].BatchData.data();
    std::byte* dstData = batches[dstSlot.BatchIndex].BatchData.data();

    const std::vector<Type>& componentsTypes = memoryLayout.GetActiveComponentsTypes();

    for(uint32_t i = 0; i < componentsTypes.size(); i++)
    {
        Type componentType = componentsTypes[i];

        size_t size = ComponentsRegistry::GetComponentSize(componentType);

        MemoryInfo memoryInfo = memoryLayout.GetMemoryInfo(componentType);

        memcpy
        (
            dstData + memoryInfo.offset + dstSlot.RowIndex * memoryInfo.stride,
            srcData + memoryInfo.offset + srcSlot.RowIndex * memoryInfo.stride,
            size
        );
    }
}

void Table::CompactBatches()
{
    uint32_t batchIndexOffset = 0;
    uint32_t rowIndexOffset = 0;
    std::cout << "Batches Count:  " << batchesCount << std::endl;
    bool isAlreadyCompacted = false;
    for (int32_t batchIndex = static_cast<int32_t>(batchesCount) - 1; batchIndex >= 0; batchIndex--)
    {
        Batch& batch = batches[batchIndex];

        for(int32_t rowIndex = static_cast<int32_t>(batch.PeakBatchSize) - 1; rowIndex >= 0; rowIndex--)
        {
            if(!batch.ActiveEntities[rowIndex]) { continue; }

            SlotInfo srcSlotInfo {static_cast<uint32_t>(batchIndex), static_cast<uint32_t>(rowIndex)};
            SlotInfo dstSlotInfo = FindFreeSlot(batchIndexOffset, rowIndexOffset);

            if((dstSlotInfo.BatchIndex == UINT32_MAX && dstSlotInfo.RowIndex == UINT32_MAX) ||
                dstSlotInfo.BatchIndex > srcSlotInfo.BatchIndex ||
                (dstSlotInfo.BatchIndex == srcSlotInfo.BatchIndex && dstSlotInfo.RowIndex > srcSlotInfo.RowIndex))
            {
                // No hole found
                isAlreadyCompacted = true;
                break;
            }

            batchIndexOffset = dstSlotInfo.BatchIndex;
            rowIndexOffset = dstSlotInfo.RowIndex + 1;

            WriteComponents(srcSlotInfo, dstSlotInfo);

            Batch& srcBatch = batches[srcSlotInfo.BatchIndex];
            Batch& dstBatch = batches[dstSlotInfo.BatchIndex];

            srcBatch.ActiveEntitiesCount--;
            srcBatch.ActiveEntities[srcSlotInfo.RowIndex] = false;

            dstBatch.ActiveEntitiesCount++;
            dstBatch.ActiveEntities[dstSlotInfo.RowIndex] = true;

            uint32_t entityID = srcBatch.EntitiesID[srcSlotInfo.RowIndex];
            dstBatch.EntitiesID[dstSlotInfo.RowIndex] = entityID;

            EntityManager::UpdateEntityRecord(entityID, dstSlotInfo.BatchIndex, dstSlotInfo.RowIndex);
        }

        if(isAlreadyCompacted)
        {
            break;
        }
    }

    for(int32_t batchIndex = static_cast<int32_t>(batchesCount) - 1; batchIndex >= 0; batchIndex--)
    {
        Batch& batch = batches[batchIndex];

        if(batch.ActiveEntitiesCount == 0) { DestroyLastBatch(); continue; }

        batch.PeakBatchSize = batch.ActiveEntitiesCount;
    }
}

SlotInfo Table::FindFreeSlot(uint32_t batchIndex, uint32_t rowIndex)
{
    for(; batchIndex < batchesCount; batchIndex++)
    {
        Batch& batch = batches[batchIndex];

        for(; rowIndex < batch.PeakBatchSize; rowIndex++)
        {
            bool isValidEntity = batch.ActiveEntities[rowIndex];

            if(isValidEntity) { continue; }

            return SlotInfo{static_cast<uint32_t>(batchIndex), static_cast<uint32_t>(rowIndex)};
        }

        rowIndex = 0;
    }

    return {UINT32_MAX,UINT32_MAX};
}

uint32_t Table::GetEntitiesCount()
{
    return entitiesCount;
}