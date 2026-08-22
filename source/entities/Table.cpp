#include <eve/entities/ComponentsRegistry.hpp>
#include <eve/entities/Table.hpp>
#include <eve/entities/EntityManager.hpp>

Table::Table(Type archtype, uint32_t batchSizeBytes) :
archtype(archtype), 
batchSizeBytes(batchSizeBytes),
memoryLayout(MemoryLayout(archtype, batchSizeBytes)),
batchesCount(0), entitiesCount(0)
{
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

    if(freeSlots.empty())
    {
        uint32_t lastBatchIndex = batches.size() - 1;

        Batch& lastBatch = batches[lastBatchIndex];

        // Is the last batch full?
        if(lastBatch.PeakBatchSize == maxEntitiesPerBatch)
        {
            CreateBatch();

            Batch& batch = batches.back();

            SlotInfo slotInfo {lastBatchIndex + 1, batch.PeakBatchSize};

            batch.PeakBatchSize++;
            batch.ActiveEntities[0] = true;
            batch.EntitiesID[0] = entityId;
            batch.ActiveEntitiesCount++;

            return slotInfo;
        }

        uint32_t rowIndex = lastBatch.PeakBatchSize;
        SlotInfo slotInfo {lastBatchIndex, rowIndex};

        lastBatch.PeakBatchSize++;
        lastBatch.ActiveEntities[rowIndex] = true;
        lastBatch.EntitiesID[rowIndex] = entityId;
        lastBatch.ActiveEntitiesCount++;

        return slotInfo;
    }

    SlotInfo& slotInfo = freeSlots.back();
    freeSlots.pop_back();

    Batch& batch = batches[slotInfo.BatchIndex];

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
    uint32_t batchesCount = batches.size();

    int32_t batchIndexOffset = static_cast<int32_t>(batchesCount) - 1;
    int32_t rowIndexOffset = static_cast<int32_t>(batches.back().PeakBatchSize) - 1;

    for(int32_t holeIndex = static_cast<int32_t>(freeSlots.size()) - 1; holeIndex >= 0; holeIndex--)
    {
        SlotInfo dstSlotInfo = freeSlots[holeIndex];

        SlotInfo srcSlotInfo = FindValidEntity(batchIndexOffset, rowIndexOffset);

        if(srcSlotInfo.BatchIndex == UINT32_MAX && srcSlotInfo.RowIndex == UINT32_MAX)
        {
            // No components found, all batches are empty
            break;
        }

        batchIndexOffset = srcSlotInfo.BatchIndex;
        rowIndexOffset = srcSlotInfo.RowIndex;

        freeSlots.pop_back();
        
        // Check if it is moving the srcSlot to the right
        if((dstSlotInfo.BatchIndex > srcSlotInfo.BatchIndex) ||
         dstSlotInfo.BatchIndex == srcSlotInfo.BatchIndex && dstSlotInfo.RowIndex >= srcSlotInfo.RowIndex)
        {
            continue;
        }

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

    if(!batches.empty())
    {
        Batch& lastBatch = batches.back();
        
        for(uint32_t i = 0; i < lastBatch.ActiveEntities.size(); i++)
        {
            if(!lastBatch.ActiveEntities[i])
            {
                lastBatch.PeakBatchSize = i + 1;
                break;
            }
        }
    }
}

SlotInfo Table::FindValidEntity(int32_t batchIndex, int32_t rowIndex)
{
    for(; batchIndex >= 0; batchIndex--)
    {
        Batch& batch = batches[batchIndex];

        if(batch.ActiveEntitiesCount == 0) { DestroyLastBatch(); continue; }

        for(; rowIndex >= 0; rowIndex--)
        {
            bool isValidEntity = batch.ActiveEntities[rowIndex];

            if(!isValidEntity) { continue; }

            return SlotInfo{static_cast<uint32_t>(batchIndex), static_cast<uint32_t>(rowIndex)};
        }

        rowIndex = static_cast<int32_t>(maxEntitiesPerBatch) - 1;
    }

    return {UINT32_MAX,UINT32_MAX};
}

uint32_t Table::GetEntitiesCount()
{
    return entitiesCount;
}