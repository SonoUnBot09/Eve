#include "EntityManager.hpp"

void EntityManager::Initialize( const uint32_t maxEntitiesCount, const uint32_t maxDestroyEntityCommands)
{
    if(isInitialized) { return; }
    if(maxEntitiesCount == 0) { return; }
    if(maxDestroyEntityCommands == 0) { return; }

    entities = new Entity[maxEntitiesCount];
    entitiesRegister = new EntityRecord[maxEntitiesCount];

    pendingDestructionEntities.reserve(maxDestroyEntityCommands);
    afterTransitionInvalidSlots.reserve(maxDestroyEntityCommands);

    newComponentsType.reserve(64);
    oldComponentsType.reserve(64);

    isInitialized = true;
}

void EntityManager::Destroy()
{
    if(!isInitialized) { return; }
    delete[] entities;
    delete[] entitiesRegister;

    for (auto &[archtype, table] : tables)
    {
        delete table;
    }
}

std::vector<Table*>& EntityManager::GetTablesFromQuery(const uint32_t queryTicket)
{
    return queryResults[queryTicket];
}

uint32_t EntityManager::RegisterQuery(const QueryInfo queryInfo)
{
    queryInfos.push_back(queryInfo);
    return queryInfos.size() - 1;
}

void EntityManager::UpdateQueries()
{
    queryResults.clear();
    queryResults.resize(queryInfos.size());
    for (auto &[archtype, table] : tables)
    {
        for (uint32_t i = 0; i < queryInfos.size(); i++)
        {
            QueryInfo queryInfo = queryInfos[i];

            if(!queryInfo.IsExclusive)
            {
                bool isTableValid = true;
                for (uint32_t j = 0; j < 64; j++)
                {
                    if(!queryInfo.ComponentsRequired.test(j)) { continue; }
                    if(!archtype.test(j)) { isTableValid = false; break; }

                }

                if(!isTableValid) { continue; }

                queryResults[i].push_back(table);
            }
            else 
            {
                if(queryInfo.ComponentsRequired == archtype)
                {
                    queryResults[i].push_back(table);
                }
            }
        }
    }
}

Table* EntityManager::GetTable(const Type archtype)
{
    auto it = tables.find(archtype);

    if(it == tables.end())
    {
        updateQueries = true;
        Table* table = new Table(archtype, 16384);
        auto [insertedIt, success] = tables.try_emplace(archtype, table);
        return (insertedIt->second);
    }

    return (it->second);
}

void EntityManager::SetEntityInfos(const uint32_t id, Table* table, const uint32_t batchIndex, const uint32_t rowIndex, const Type archtype)
{ 
    table->SetEntityIndex(batchIndex, rowIndex, id);
    table->SetEntityHolesBitIndex(batchIndex, rowIndex, false);
    
    entitiesRegister[id] = {
        table,
        batchIndex,
        rowIndex,
        archtype
    };
}

void EntityManager::RegisterEntityCommandPool(EntityCommandPool& entityCommandPool)
{
    entityCommandPools.push_back(&entityCommandPool);
}

void EntityManager::ExecuteEntityCommands()
{
    for(EntityCommandPool* entityCommandPool : entityCommandPools)
    {
        // Fill the pendingDestructionEntities vector with all
        // the entities which need to be destroyed
        std::vector<EntityDestructionCommand>& destructionCommands = entityCommandPool->GetDestructionCommands();
        for (uint32_t i = 0; i < destructionCommands.size(); i++)
        {
            EntityDestructionCommand command = destructionCommands[i];
            uint32_t id = command.Id;
            uint32_t generationId = command.GenerationId;

            if(generationId != entities[id].GenerataionId) { continue; }

            EntityRecord& entityRecord = entitiesRegister[id];

            SlotInfo entityInfo 
            {
                id, 
                generationId,
                entityRecord.Table,
                entityRecord.BatchIndex,
                entityRecord.RowIndex,
                entityRecord.Archtype,
                true
            };

            pendingDestructionEntities.push_back(entityInfo);
            entityRecord.Table->SetEntityHolesBitIndex(entityRecord.BatchIndex, entityRecord.RowIndex, true);
        }
    }

    SortEntitiesToDestroy(pendingDestructionEntities);
    
    for(EntityCommandPool* entityCommandPool : entityCommandPools)
    {
        //transition
        ExecuteTransitionCommands(entityCommandPool);
    }
    
    for(EntityCommandPool* entityCommandPool : entityCommandPools)
    {
        //creation
        ExecuteCreationCommands(entityCommandPool);
    }

    pendingDestructionEntities.insert
    (
        pendingDestructionEntities.end(),
        afterTransitionInvalidSlots.begin(), 
        afterTransitionInvalidSlots.end()
    );

    afterTransitionInvalidSlots.clear();

    SortEntitiesToDestroy(pendingDestructionEntities);

    pendingDestructionEntities.clear();

    CompactBatches();

    if(updateQueries)
    {
        UpdateQueries();
    }
}

void EntityManager::ExecuteTransitionCommands(EntityCommandPool* entityCommandPool)
{
    std::vector<EntityTransitionCommand>& entityTransitionCommands = entityCommandPool->GetTransitionCommands();
    std::vector<std::byte>& creationComponentsData = entityCommandPool->GetTransitionComponentsData();

    Type previousOldArchtype = 0;
    Type previousNewArchtype = 0;
    
    for (uint32_t i = 0; i < entityTransitionCommands.size(); i++)
    {
        EntityTransitionCommand& command = entityTransitionCommands[i];
        uint32_t id = command.Id;
        uint32_t generationId = command.GenerationId;

        // Is Entity valid?
        if(generationId != entities[id].GenerataionId) { continue; }

        EntityRecord& oldSlot = entitiesRegister[id];
        Type validComponentsTypeFromOldArchtype = (oldSlot.Archtype & ~command.DestroyComponentsType);

        if(previousOldArchtype != validComponentsTypeFromOldArchtype)
        {
            oldComponentsType.clear();
            // Get valid old components type
            for (uint32_t j = 0; j < 64; j++)
            {
                if(!validComponentsTypeFromOldArchtype.test(j)) { continue; }
                
                Type type = 0;
                type.set(j);
                oldComponentsType.push_back(type);
                
            }

            previousOldArchtype = validComponentsTypeFromOldArchtype;
        }

        if(previousNewArchtype != command.CreateComponentsType)
        {
            newComponentsType.clear();
            // Get new components type
            for (uint32_t j = 0; j < 64; j++)
            {
                if(!command.CreateComponentsType.test(j)) { continue; }
                
                Type type = 0;
                type.set(j);
                newComponentsType.push_back(type);
                
            }

            previousNewArchtype = command.CreateComponentsType;
        }

        Type newArchtype = validComponentsTypeFromOldArchtype | command.CreateComponentsType;

        //Try to re-use the destroyed entity slots
        int32_t newSlotIndex = TryGetAvailableSlot(newArchtype);

        Table* oldTable = oldSlot.Table;
        Table* newTable = nullptr;

        std::byte* oldBatch = oldTable->GetComponentsBatch(oldSlot.BatchIndex);

        uint32_t newBatchIndex = 0;
        std::byte* newBatch = nullptr;
        uint32_t newRowIndex = 0;

        if(newSlotIndex == -1)
        {
            // No slot available, needs to allocate in a new space
            newTable = GetTable(newArchtype);

            std::tuple<uint32_t, uint32_t> slotInfo = newTable->GetNewEntitySlot();

            newBatchIndex = std::get<0>(slotInfo);
            newRowIndex = std::get<1>(slotInfo);
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }
        else 
        {
            SlotInfo& newSlot = pendingDestructionEntities[newSlotIndex];
            // Slot available
            newTable = newSlot.Table;

            newBatchIndex = newSlot.BatchIndex;
            newRowIndex = newSlot.RowIndex;
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }

        for (uint32_t j = 0; j < oldComponentsType.size(); j++)
        {
            Type componentType = oldComponentsType[j];
            const MemoryInfo* oldMemoryInfo = oldTable->GetMemoryInfo(componentType);
            const MemoryInfo* newMemoryInfo = newTable->GetMemoryInfo(componentType);

            std::memcpy(
                newBatch + newMemoryInfo->offset + newMemoryInfo->stride * newRowIndex,
                oldBatch + oldMemoryInfo->offset + oldMemoryInfo->stride * oldSlot.RowIndex,
                ComponentsRegistry::GetComponentSizeFromBit(componentType)
            );
        }

        uint32_t offset = 0;
        for (uint32_t j = 0; j < newComponentsType.size(); j++)
        {
            Type componentType = newComponentsType[j];
            const MemoryInfo* newMemoryInfo = newTable->GetMemoryInfo(componentType);
            
            uint32_t size = ComponentsRegistry::GetComponentSizeFromBit(componentType);

            std::memcpy
            (
                newBatch + newMemoryInfo->offset + newMemoryInfo->stride * newRowIndex,
                creationComponentsData.data() + command.ComponentOffset + offset,
                size
            );

            offset += size;
        }

        SetEntityInfos(id, newTable, newBatchIndex, newRowIndex, newArchtype);
        oldTable->SetEntityHolesBitIndex(oldSlot.BatchIndex, oldSlot.RowIndex, true);
        
        uint32_t oldTableComponentsCount = oldTable->GetComponentsCountPerBatch(oldSlot.BatchIndex);
        uint32_t newTableComponentsCount = newTable->GetComponentsCountPerBatch(newBatchIndex);

        oldTable->SetComponentsCountPerBatch(oldSlot.BatchIndex, oldTableComponentsCount - 1);
        newTable->SetComponentsCountPerBatch(newBatchIndex, newTableComponentsCount + 1);
        
        afterTransitionInvalidSlots.emplace_back(
            0,
            0,
            oldTable,
            oldSlot.BatchIndex,
            oldSlot.RowIndex,
            oldSlot.Archtype,
            true
        );
    }
}

void EntityManager::ExecuteCreationCommands(EntityCommandPool* entityCommandPool)
{
    std::vector<EntityCreationCommand>& entityCreationCommands = entityCommandPool->GetCreationCommands();
    std::vector<std::byte>& creationComponentsData = entityCommandPool->GetCreationComponentsData();
    
    Type previousArchtype = 0;
    for (uint32_t i = 0; i < entityCreationCommands.size(); i++)
    {
        EntityCreationCommand& command = entityCreationCommands[i];
        
        if(previousArchtype != command.Archtype)
        {
            newComponentsType.clear();
            // Get new components type
            for (uint32_t j = 0; j < 64; j++)
            {
                if(!command.Archtype.test(j)) { continue; }
                
                Type type = 0;
                type.set(j);
                newComponentsType.push_back(type);
            }

            previousArchtype = command.Archtype;
        }
        
        int32_t newSlotIndex = TryGetAvailableSlot(command.Archtype);
        
        Table* newTable = nullptr;

        uint32_t newBatchIndex = 0;
        uint32_t newRowIndex = 0;
        std::byte* newBatch = nullptr;
        
        if(newSlotIndex == -1)
        {
            // No slot available, needs to allocate in a new space
            newTable = GetTable(command.Archtype);
            
            std::tuple<uint32_t, uint32_t> slotInfo = newTable->GetNewEntitySlot();

            newBatchIndex = std::get<0>(slotInfo);
            newRowIndex = std::get<1>(slotInfo);
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }
        else 
        {
            SlotInfo& newSlot = pendingDestructionEntities[newSlotIndex];

            newTable = newSlot.Table;

            // Slot available
            newBatchIndex = newSlot.BatchIndex;
            newRowIndex = newSlot.RowIndex;
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }
        
        uint32_t offset = 0;
        for (uint32_t j = 0; j < newComponentsType.size(); j++)
        {
            Type componentType = newComponentsType[j];
            const MemoryInfo* memoryInfo = newTable->GetMemoryInfo(componentType);

            const uint32_t size = ComponentsRegistry::GetComponentSizeFromBit(componentType);

            std::memcpy
            (
                newBatch + memoryInfo->offset + memoryInfo->stride * newRowIndex,
                creationComponentsData.data() + command.ComponentOffset + offset,
                size
            );

            offset += size;
        }

        SetEntityInfos(command.Id, newTable, newBatchIndex, newRowIndex, command.Archtype);
        
        uint32_t newTableComponentsCount = newTable->GetComponentsCountPerBatch(newBatchIndex);
        newTable->SetComponentsCountPerBatch(newBatchIndex, newTableComponentsCount + 1);
        
        entities[command.Id] = {command.Id, command.GenerationId};
    }
}

void EntityManager::CompactBatches()
{
    Type previousArchtype = 0;
    for (uint32_t i = 0; i < pendingDestructionEntities.size(); i++)
    {   
        bool skip = false;
        SlotInfo& holeSlot = pendingDestructionEntities[i];

        if(!holeSlot.IsHole) { continue; }

        Table* table = holeSlot.Table;
        Type archtype = holeSlot.Archtype;

        if(previousArchtype != archtype)
        {
            newComponentsType.clear();
            for (uint32_t bit = 0; bit < 64; bit++)
            {
                if(!archtype.test(bit)) { continue; }
                
                Type type = 0;
                type.set(bit);
                newComponentsType.push_back(type);
            }

            previousArchtype = archtype;
        }

        for (int32_t batchIndex = (int32_t)(table->GetLastBatchIndex()); batchIndex >= 0; batchIndex--)
        {
            std::vector<bool>& holesBit = table->holesBit[batchIndex];
            for (int32_t j = (int32_t)(table->GetMaxComponentsCountPerBatch() - 1); j >= 0; j--)
            {
                if(holesBit[j]) { continue; }

                if(holeSlot.BatchIndex > batchIndex || 
                    (holeSlot.BatchIndex == batchIndex && holeSlot.RowIndex > j))
                {
                    uint32_t componentsCount = table->GetComponentsCountPerBatch(holeSlot.BatchIndex);
                    table->SetComponentsCountPerBatch(holeSlot.BatchIndex, componentsCount - 1);
                    skip = true;
                    break;
                }
                
                skip = true;

                std::byte* srcBatch = table->GetComponentsBatch(batchIndex);
                std::byte* dstBatch = table->GetComponentsBatch(holeSlot.BatchIndex);
                for (uint32_t w = 0; w < newComponentsType.size(); w++)
                {
                    Type componentType = newComponentsType[w];

                    const MemoryInfo* memoryInfo = table->GetMemoryInfo(componentType);

                    uint32_t size = ComponentsRegistry::GetComponentSizeFromBit(componentType);
                    std::memcpy
                    (
                        dstBatch + memoryInfo->offset + memoryInfo->stride * holeSlot.RowIndex,
                        srcBatch + memoryInfo->offset + memoryInfo->stride * j,
                        size
                    );
                }

                uint32_t entityId = table->entitiesIndices[batchIndex][j];
                SetEntityInfos(entityId, table, holeSlot.BatchIndex, holeSlot.RowIndex, holeSlot.Archtype);

                table->SetEntityHolesBitIndex(batchIndex, j, true);

                uint32_t componentsCountOldBatch = table->GetComponentsCountPerBatch(batchIndex);
                table->SetComponentsCountPerBatch(batchIndex, componentsCountOldBatch - 1);

                break;
            }

            if(skip) { break; }
        }

    }

    // Checks for empty batches and tables
    for (auto it = tables.begin(); it != tables.cend();)
    {
        Type archtype = it->first;
        Table* table = it->second;

        uint32_t batchesCount = table->batches.size();

        for(int32_t i = (int32_t)batchesCount - 1; i >= 0; i--)
        {
            uint32_t componentsCount = table->GetComponentsCountPerBatch(i);

            if(componentsCount != 0) { break; }

            table->DeallocateBatch(i);
            batchesCount--;
        }

        if(batchesCount == 0)
        {
            updateQueries = true;
            delete table;
            tables.erase(archtype);
        }

        it++;
    }
}

void EntityManager::SortEntitiesToDestroy(std::vector<SlotInfo>& vector)
{
    // Sort the pendingDestructionEntities vector based on their archtype
    std::sort(
        pendingDestructionEntities.begin(),
        pendingDestructionEntities.end(),
        [](const SlotInfo& a, const SlotInfo& b)
        {
            if(a.Archtype != b.Archtype)
            {
                return a.Archtype.to_ullong() < b.Archtype.to_ullong();
            }

            if (a.BatchIndex != b.BatchIndex)
            {
                return a.BatchIndex > b.BatchIndex;
            }

            return a.RowIndex > b.RowIndex;
        }
    );
}

int32_t EntityManager::TryGetAvailableSlot(const Type archtype)
{
    SlotInfo value;
    value.Archtype = archtype;

    auto range = std::equal_range(
        pendingDestructionEntities.begin(),
        pendingDestructionEntities.end(),
        value,
        [](const SlotInfo& a, const SlotInfo& b)
        {
            return a.Archtype.to_ullong() < b.Archtype.to_ullong();
        }
    );

    for (auto it = range.first; it != range.second; ++it)
    {
        if(it->IsHole)
        {
            it->IsHole = false;

            int32_t index = std::distance(pendingDestructionEntities.begin(), it);
            return index;
        }
    }

    // No available slot
    return -1;
}