#include "EntityManager.hpp"

Table* EntityManager::GetTable(const Type archtype)
{
    return &componentsPools.at(archtype);
}

void EntityManager::RecordEntityCommandPool(EntityCommandPool& entityCommandPool)
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
            uint32_t id = destructionCommands[i].entityId;
            uint32_t generationId = entities[id].GenerataionId;

            if(generationId != destructionCommands[i].entityGeneration) { continue; }

            EntityRecord entityRecord = entitiesRegister[id];

            EntityInfo entityInfo{id, entityRecord.Archtype, entityRecord.BatchIndex, entityRecord.RowIndex, true};
            pendingDestructionEntities.push_back(entityInfo);

            Table* table = GetTable(entityRecord.Archtype);
            table->SetEntitiesHolesBitIndex(entityRecord.BatchIndex, entityRecord.RowIndex, true);
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
        invalidEntities.begin(), 
        invalidEntities.end()
    );

    invalidEntities.clear();

    CompactBatches();

}

void EntityManager::ExecuteTransitionCommands(EntityCommandPool* entityCommandPool)
{
    std::vector<EntityTransitionCommand>& entityTransitionCommands = entityCommandPool->GetTransitionCommands();
    std::vector<std::byte>& creationComponentsData = entityCommandPool->GetTransitionComponentsData();

    for (uint32_t i = 0; i < entityTransitionCommands.size(); i++)
    {
        EntityTransitionCommand& command = entityTransitionCommands[i];
        Entity entity = entities[command.entityId];

        // Is Entity valid?
        if(entity.GenerataionId != command.entityGeneration) { continue; }

        EntityRecord& oldSlot = entitiesRegister[command.entityId];

        Type validComponentsTypeFromOldArchtype = (oldSlot.Archtype & ~command.destroyComponentsType);

        // Get valid old components type
        for (uint32_t j = 0; j < 64; j++)
        {
            if(!validComponentsTypeFromOldArchtype.test(j)) { continue; }
            
            Type type = 0;
            type.set(j);
            oldComponentsType.push_back(type);
            
        }
        // Get new components type
        for (uint32_t j = 0; j < 64; j++)
        {
            if(!command.createComponentsType.test(j)) { continue; }
            
            Type type = 0;
            type.set(j);
            newComponentsType.push_back(type);
            
        }

        Type newArchtype = validComponentsTypeFromOldArchtype | command.createComponentsType;

        EntityInfo* newSlot = TryGetAvailableSlot(newArchtype);

        Table* oldTable = GetTable(oldSlot.Archtype);
        Table* newTable = GetTable(newArchtype);

        std::byte* oldBatch = oldTable->GetComponentsBatch(oldSlot.BatchIndex);

        uint32_t newBatchIndex = 0;
        std::byte* newBatch = nullptr;
        uint32_t rowIndex = 0;

        if(newSlot == nullptr)
        {
            // No slot available, needs to allocate in a new space
            rowIndex = newTable->GetNewSlot();
            newBatchIndex = newTable->GetLastBatchIndex();
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }
        else 
        {
            // Slot available
            rowIndex = newSlot->RowIndex;
            newBatchIndex = newSlot->BatchIndex;
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }

        for (uint32_t j = 0; j < oldComponentsType.size(); j++)
        {
            Type componentType = oldComponentsType[j];
            const MemoryInfo* oldMemoryInfo = oldTable->GetMemoryInfo(componentType);
            const MemoryInfo* newMemoryInfo = newTable->GetMemoryInfo(componentType);

            std::memcpy(
                newBatch + newMemoryInfo->offset + newMemoryInfo->stride * rowIndex,
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
                newBatch + newMemoryInfo->offset + newMemoryInfo->stride * rowIndex,
                creationComponentsData.data() + command.componentOffset + offset,
                size
            );

            offset += size;
        }

        oldTable->SetEntitiesHolesBitIndex(oldSlot.BatchIndex, oldSlot.RowIndex, true);
        newTable->SetEntitiesHolesBitIndex(newBatchIndex, rowIndex, false);
        
        newTable->SetEntitiesIndex(newBatchIndex, rowIndex, command.entityId);

        uint32_t oldTableComponentsCount = oldTable->GetComponentsCountPerBatch(oldSlot.BatchIndex);
        uint32_t newTableComponentsCount = newTable->GetComponentsCountPerBatch(newBatchIndex);

        oldTable->SetComponentsCountPerBatch(oldSlot.BatchIndex, oldTableComponentsCount - 1);
        newTable->SetComponentsCountPerBatch(newBatchIndex, newTableComponentsCount + 1);
        
        invalidEntities.emplace_back(
            command.entityId,
            oldSlot.Archtype,
            oldSlot.BatchIndex,
            oldSlot.RowIndex,
            true
        );

        entitiesRegister[command.entityId] = {newArchtype, newBatchIndex, rowIndex};

        newComponentsType.clear();
        oldComponentsType.clear();
    }
}

void EntityManager::ExecuteCreationCommands(EntityCommandPool* entityCommandPool)
{
    std::vector<EntityCreationCommand>& entityCreationCommands = entityCommandPool->GetCreationCommands();
    std::vector<std::byte>& creationComponentsData = entityCommandPool->GetCreationComponentsData();

    for (uint32_t i = 0; i < entityCreationCommands.size(); i++)
    {
        EntityCreationCommand& command = entityCreationCommands[i];

        // Get new components type
        for (uint32_t j = 0; j < 64; j++)
        {
            if(!command.archtype.test(j)) { continue; }
            
            Type type = 0;
            type.set(j);
            newComponentsType.push_back(type);
            
        }

        Table* newTable = GetTable(command.archtype);
        EntityInfo* newSlot = TryGetAvailableSlot(command.archtype);
        
        uint32_t newBatchIndex = 0;
        std::byte* newBatch = nullptr;
        uint32_t rowIndex = 0;
        if(newSlot == nullptr)
        {
            // No slot available, needs to allocate in a new space
            rowIndex = newTable->GetNewSlot();
            newBatchIndex = newTable->GetLastBatchIndex();
            newBatch = newTable->GetComponentsBatch(newBatchIndex);
        }
        else 
        {
            // Slot available
            rowIndex = newSlot->RowIndex;
            newBatchIndex = newSlot->BatchIndex;
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
                newBatch + memoryInfo->offset + memoryInfo->stride * rowIndex,
                creationComponentsData.data() + command.componentOffset + offset,
                size
            );

            offset += size;
        }
        
        newTable->SetEntitiesHolesBitIndex(newBatchIndex, rowIndex, false);

        newTable->SetEntitiesIndex(newBatchIndex, rowIndex, command.entityId);

        uint32_t newTableComponentsCount = newTable->GetComponentsCountPerBatch(newBatchIndex);
        newTable->SetComponentsCountPerBatch(newBatchIndex, newTableComponentsCount + 1);

        entities[command.entityId] = {command.entityId, command.entityGeneration};
        entitiesRegister[command.entityId] = {command.archtype, newBatchIndex, rowIndex};

        newComponentsType.clear();
    }
}

void EntityManager::CompactBatches()
{
    for (uint32_t i = 0; i < pendingDestructionEntities.size(); i++)
    {   
        bool skip = false;
        EntityInfo& entitySlot = pendingDestructionEntities[i];

        if(!entitySlot.Valid) { continue; }

        Type archtype = entitySlot.Archtype;
        Table* table = GetTable(archtype);

        for (uint32_t bit = 0; bit < 64; bit++)
        {
            if(!archtype.test(bit)) { continue; }
            
            Type type = 0;
            type.set(bit);
            newComponentsType.push_back(type);
            
        }

        for (int32_t batchIndex = (int32_t)(table->batchSize - 1); batchIndex >= 0; batchIndex--)
        {
            std::vector<bool>& holesBit = table->holesBit[batchIndex];
            for (int32_t j = (int32_t)(table->GetMaxComponentsCountPerBatch() - 1); j >= 0; j--)
            {
                if(holesBit[j]) { continue; }
                
                skip = true;

                std::byte* srcBatch = table->GetComponentsBatch(batchIndex);
                std::byte* dstBatch = table->GetComponentsBatch(entitySlot.BatchIndex);
                for (uint32_t w = 0; w < newComponentsType.size(); w++)
                {
                    Type componentType = newComponentsType[w];

                    const MemoryInfo* memoryInfo = table->GetMemoryInfo(componentType);

                    uint32_t size = ComponentsRegistry::GetComponentSizeFromBit(componentType);
                    std::memcpy
                    (
                        dstBatch + memoryInfo->offset + memoryInfo->stride * entitySlot.RowIndex,
                        srcBatch + memoryInfo->offset + memoryInfo->stride * j,
                        size
                    );
                }

                table->SetEntitiesHolesBitIndex(batchIndex, j, true);
                table->SetEntitiesHolesBitIndex(entitySlot.BatchIndex, entitySlot.RowIndex, false);

                uint32_t entityId = table->entitiesIndices[batchIndex][j];

                uint32_t componentsCountOldBatch = table->GetComponentsCountPerBatch(batchIndex);
                table->SetComponentsCountPerBatch(batchIndex, componentsCountOldBatch - 1);

                uint32_t compontntsCountNewBatch = table->GetComponentsCountPerBatch(entitySlot.BatchIndex);
                table->SetComponentsCountPerBatch(entitySlot.BatchIndex, compontntsCountNewBatch + 1);
                
                table->entitiesIndices[entitySlot.BatchIndex][entitySlot.RowIndex] = entityId;
                entitiesRegister[entityId] = {entitySlot.Archtype, entitySlot.BatchIndex, entitySlot.RowIndex};

                break;
            }

            if(skip) { break; }
        }

        newComponentsType.clear();

    }
}

void EntityManager::SortEntitiesToDestroy(std::vector<EntityInfo>& vector)
{
    // Sort the pendingDestructionEntities vector based on their archtype
    std::sort(
        pendingDestructionEntities.begin(),
        pendingDestructionEntities.end(),
        [](const EntityInfo& a, const EntityInfo& b)
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

EntityInfo* EntityManager::TryGetAvailableSlot(const Type archtype)
{
    EntityInfo value;
    value.Archtype = archtype;

    auto range = std::equal_range(
        pendingDestructionEntities.begin(),
        pendingDestructionEntities.end(),
        value,
        [](const EntityInfo& a, const EntityInfo& b)
        {
            return a.Archtype.to_ullong() < b.Archtype.to_ullong();
        }
    );

    for (auto it = range.first; it != range.second; ++it)
    {
        if(it->Valid)
        {
            it->Valid = false;
            return &(*it);
        }
    }

    return nullptr;
}

void EntityManager::FreeAllMemory()
{
    componentsPools.clear();
}