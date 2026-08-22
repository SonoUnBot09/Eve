#include <eve/entities/Entity.hpp>
#include <eve/entities/EntityManager.hpp>

using namespace Eve::Entities;

Entity EntityManager::RequestNewEntity()
{
    Entity entity;
    if(!freeEntitySlots.empty())
    {
        uint32_t id = freeEntitySlots.back();
        entities[id].GeneratationId++;

        entity = entities[id];
        activeEntities[id] = true;
    }
    else 
    {
        uint32_t entitiesCount = entities.size();

        entities.push_back({entitiesCount,0});
        entityRecords.push_back({});
        activeEntities.push_back(true);

        entity = entities.back();
    }

    return entity;
}

void EntityManager::UpdateEntityRecord(uint32_t entityId, uint32_t batchIndex, uint32_t rowIndex)
{
    entityRecords[entityId].BatchIndex = batchIndex;
    entityRecords[entityId].RowIndex = rowIndex;
}

void EntityManager::DestroyEntity(Entity entity)
{
    activeEntities[entity.Id] = false;

    EntityRecord& record = entityRecords[entity.Id];

    Table& table = GetTable(record.Archtype);

    SlotInfo slotInfo {record.BatchIndex, record.RowIndex};

    table.FreeSlot(slotInfo);
}

void EntityManager::CreateTable(Type archtype, uint32_t batchSizeBytes)
{
    updateQueries = true;
    tables.emplace(archtype, Table{archtype, batchSizeBytes});
}

Table& EntityManager::GetTable(Type archtype)
{
    auto it = tables.find(archtype);

    if(it == tables.end())
    {
        // The table does not exist, need to create a new one
        CreateTable(archtype, defaultBatchSizeBytes);

        return tables.at(archtype);
    }

    return it->second;
}

void EntityManager::ExecuteAllCommandPools()
{
    // --- Destruction Commands ---
    for(EntityCommandPool& entityCommandPool : commandPools)
    {
        std::vector<EntityDestructionCommand>& destructionCommands = entityCommandPool.GetDestructionCommands();
        for(uint32_t i = 0; i < destructionCommands.size(); i++)
        {
            EntityDestructionCommand& command = destructionCommands[i];

            Entity entity {command.Id, command.GenerationId};

            if(entities[entity.Id].GeneratationId != entity.GeneratationId) { continue; }

            DestroyEntity(entity);
        }
    }

    // --- Transition Commands ---
    Type previousOldArchtype = 0;
    Type previousNewArchtype = 0;
    for(EntityCommandPool& entityCommandPool : commandPools)
    {
        std::vector<EntityTransitionCommand>& transitionCommands = entityCommandPool.GetTransitionCommands();
        std::vector<std::byte>& creationComponentsData = entityCommandPool.GetTransitionComponentsData();

        for(uint32_t i = 0; i < transitionCommands.size(); i++)
        {
            EntityTransitionCommand& command = transitionCommands[i];

            Entity entity {command.Id, command.GenerationId};

            if(entities[entity.Id].GeneratationId != entity.GeneratationId) { continue; }

            EntityRecord& entityRecord = entityRecords[entity.Id];

            Table& oldTable = *entityRecord.Table;

            Type validComponentsTypeFromOldArchtype = (entityRecord.Archtype & ~command.DestroyComponentsType);

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

            Table& table = GetTable(newArchtype);
            SlotInfo slotInfo = table.GetNewSlot(entity.Id);

            for(uint32_t i = 0; i < oldComponentsType.size(); i++)
            {
                Type componentType = oldComponentsType[i];
                void* ptr = oldTable.GetComponentPtr({entityRecord.BatchIndex, entityRecord.RowIndex}, componentType);

                componentTypesToCopy.push_back(componentType);
                sources.push_back(ptr);
            }

            size_t offset = 0;
            for(uint32_t i = 0; i < newComponentsType.size(); i++)
            {
                Type componentType = newComponentsType[i];

                size_t size = ComponentsRegistry::GetComponentSize(componentType);

                void* ptr = creationComponentsData.data() + command.ComponentOffset + offset;

                componentTypesToCopy.push_back(componentType);
                sources.push_back(ptr);

                offset += size;
            }

            table.WriteComponents(slotInfo, componentTypesToCopy, sources);

            oldTable.FreeSlot({entityRecord.BatchIndex, entityRecord.RowIndex});

            entityRecord.Table = &table;
            entityRecord.BatchIndex = slotInfo.BatchIndex;
            entityRecord.RowIndex = slotInfo.RowIndex;
            entityRecord.Archtype = newArchtype;

            componentTypesToCopy.clear();
            sources.clear();
        }
    }

    // --- Creation Commands ---
    Type previousArchtype = 0;
    for(EntityCommandPool& entityCommandPool : commandPools)
    {
        std::vector<EntityCreationCommand>& entityCreationCommands = entityCommandPool.GetCreationCommands();
        std::vector<std::byte>& creationComponentsData = entityCommandPool.GetCreationComponentsData();

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

            Table& table = GetTable(command.Archtype);

            SlotInfo slotInfo = table.GetNewSlot(command.Id);

            size_t offset = 0;
            for(uint32_t i = 0; i < newComponentsType.size(); i++)
            {
                Type componentType = newComponentsType[i];

                size_t size = ComponentsRegistry::GetComponentSize(componentType);

                void* ptr = creationComponentsData.data() + command.ComponentOffset + offset;

                componentTypesToCopy.push_back(componentType);
                sources.push_back(ptr);

                offset += size;
            }

            table.WriteComponents(slotInfo, componentTypesToCopy, sources);

            EntityRecord record = entityRecords[command.Id];

            record.Table = &table;
            record.BatchIndex = slotInfo.BatchIndex;
            record.RowIndex = slotInfo.RowIndex;
            record.Archtype = command.Archtype;

            componentTypesToCopy.clear();
            sources.clear();

        }

    }

    for(auto table : tables)
    {
        table.second.CompactBatches();
    }

    if(updateQueries)
    {
        for(auto& query : tableQueries)
        {
            UpdateQuery(query.first, query.second);
        }

        updateQueries = false;
    }

    ClearAllCommandPools();
}

EntityCommandPool& EntityManager::GetAvailableCommandPool(uint32_t systemID)
{
    if(systemID < commandPools.size())
    {
        return commandPools[systemID];
    }

    commandPools.emplace_back(EntityCommandPool{});
    return commandPools.back();
}

void EntityManager::ClearAllCommandPools()
{
    for(uint32_t i = 0; i < commandPools.size(); i++)
    {
        EntityCommandPool& commandPool = commandPools[i];
        
        commandPool.Clear();
    }
}

const std::vector<Table*>& EntityManager::GetTablesFromQuery(QueryInfo queryInfo)
{
    auto it = tableQueries.find(queryInfo);

    if(it == tableQueries.end())
    {
        tableQueries[queryInfo] = std::vector<Table*>();

        std::vector<Table*>& results =  tableQueries[queryInfo];

        UpdateQuery(queryInfo, results);

        return results;
    }

    return it->second;
}

void EntityManager::UpdateQuery(QueryInfo queryInfo, std::vector<Table*>& result)
{
    result.clear();

    for (auto& [archtype, table] : tables)
    {
        if(!queryInfo.IsExclusive)
        {
            bool isTableValid = true;
            for (uint32_t j = 0; j < 64; j++)
            {
                if(!queryInfo.ComponentsRequired.test(j)) { continue; }
                if(!archtype.test(j)) { isTableValid = false; break; }

            }

            if(!isTableValid) { continue; }

            result.push_back(&table);
        }
        else 
        {
            if(queryInfo.ComponentsRequired == archtype)
            {
                result.push_back(&table);
            }
        }
    }
}

Entity EntityManager::ScheduleCreationCommand(EntityCommandInfo* commandInfo, uint32_t systemId)
{
    EntityCommandPool& commandPool = GetAvailableCommandPool(systemId);

    return commandPool.ScheduleCreationCommand(commandInfo);
}
void EntityManager::ScheduleDestructionCommand(const Entity entity, uint32_t systemId)
{
    EntityCommandPool& commandPool = GetAvailableCommandPool(systemId);

    commandPool.ScheduleDestructionCommand(entity);
}
void EntityManager::ScheduleTransitionCommand(const Entity entity, EntityCommandInfo& commandInfo, uint32_t systemId)
{
    EntityCommandPool& commandPool = GetAvailableCommandPool(systemId);

    commandPool.ScheduleTransitionCommand(entity, commandInfo);
}