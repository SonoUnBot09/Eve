#pragma once

#include "QueryResult.hpp"
#include <unordered_map>

#include <eve/entities/Type.hpp>
#include <eve/entities/Entity.hpp>
#include <eve/entities/Table.hpp>
#include <eve/entities/details/EntityCommandPool.hpp>

#include <eve/entities/QueryInfo.hpp>

namespace Eve
{
    class Application;
}

namespace Eve::Entities
{

    class EntityManager
    {
        public:

            static void CreateTable(Type archtype, uint32_t batchSizeBytes);

            static QueryResult& GetTables(QueryInfo queryInfo);

            static Entity ScheduleCreationCommand(EntityCommandInfo* commandInfo, uint32_t systemId);
            static void ScheduleDestructionCommand(const Entity entity, uint32_t systemId);
            static void ScheduleTransitionCommand(const Entity entity, EntityCommandInfo& commandInfo, uint32_t systemId);

        private:

            static void Initialize(uint32_t preAllocEntitySize);

            static constexpr uint32_t defaultBatchSizeBytes = 16384; // 16 KB

            static EntityCommandPool& GetAvailableCommandPool(uint32_t systemID);
            static void ClearAllCommandPools();
            static void ExecuteAllCommandPools();

            static Entity GetEntity(uint32_t id);
            static Entity RequestNewEntity();
            static void DestroyEntity(Entity entity);

            static void UpdateEntityRecord(uint32_t entityId, uint32_t batchIndex, uint32_t rowIndex);

            static void UpdateQuery(QueryInfo queryInfo, QueryResult& result);

            static Table& GetTable(Type archtyte);

            inline static std::unordered_map<Type, Table> tables;

            inline static std::vector<EntityCommandPool> commandPools;

            inline static std::unordered_map<QueryInfo, QueryResult> tableQueries;
            inline static bool updateQueries = false;

            inline static std::vector<bool> activeEntities;
            inline static std::vector<Entity> entities;
            inline static std::vector<EntityRecord> entityRecords;

            inline static std::vector<uint32_t> freeEntitySlots;

            #pragma region Internal
            inline static std::vector<Type> newComponentsType;
            inline static std::vector<Type> oldComponentsType;
            
            inline static std::vector<void*> sources;
            inline static std::vector<Type> componentTypesToCopy;
            #pragma endregion

            friend class Table;
            friend class EntityCommandPool;
            friend class Eve::Application;
    };
}