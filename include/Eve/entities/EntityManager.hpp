#pragma once

#include <functional>
#include <unordered_map>

#include <eve/entities/Type.hpp>
#include <eve/entities/Entity.hpp>
#include <eve/entities/EntityCommandPool.hpp>

#include <eve/entities/QueryInfo.hpp>

namespace Eve::Entities
{
    class EntityManager
    {
        public:

            EntityManager(uint32_t preAllocEntitySize)
            {
                activeEntities.reserve(preAllocEntitySize);
                entities.reserve(preAllocEntitySize);
                entityRecords.reserve(preAllocEntitySize);
            }

            static Entity RequestNewEntity();

            static void UpdateEntityRecord(uint32_t entityId, uint32_t batchIndex, uint32_t rowIndex);
            
            static const std::vector<Table*>& GetTablesFromQuery(QueryInfo queryInfo);

        private:

            static constexpr uint32_t defaultBatchSizeBytes = 16384;

            static EntityCommandPool& GetAvailableCommandPool(uint32_t systemID);
            static void ClearAllCommandPools();
            static void ExecuteAllCommandPools();

            static void DestroyEntity(Entity entity);

            static void CreateTable(Type archtype, uint32_t batchSizeBytes);
            static Table& GetTable(Type archtyte);

            static void UpdateQuery(QueryInfo queryInfo, std::vector<Table*>& result);

            inline static std::unordered_map<Type, Table> tables;

            inline static std::vector<EntityCommandPool> commandPools;

            inline static std::unordered_map<QueryInfo, std::vector<Table*>> tableQueries;
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
    };
}