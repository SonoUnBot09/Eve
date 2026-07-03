#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>

#include <Eve/Entities/Type.hpp>
#include <Eve/Entities/Entity.hpp>
#include <Eve/Entities/ComponentsRegistry.hpp>
#include <Eve/Entities/Table.hpp>
#include <Eve/Entities/EntityCommandPool.hpp>
#include <Eve/Entities/EntityCommands.hpp>
#include <Eve/Entities/QueryInfo.hpp>
#include <Eve/Entities/SlotInfo.hpp>

namespace Eve::Entities
{
    class EntityManager
    {
        public:
            //10000, 500
            static void Initialize(const uint32_t maxEntityCount = 100001, const uint32_t maxEntityCommandsCount = 1);
            static void Destroy();

            // Queries
            static uint32_t RegisterQuery(const QueryInfo queryInfo);
            static std::vector<std::reference_wrapper<Table>>& GetTablesFromQuery(const uint32_t queryTicket);

            // Entity Command Pools
            static uint32_t CreateCommandPool(const uint32_t creationCommandBufferInitialSize = 0, const uint32_t destructionCommandBufferInitialSize = 0, 
                const uint32_t transitionCommandBufferInitialSize = 0, const uint32_t creationComponentsInitialSize = 0, 
                const uint32_t transitionComponentsInitialSize = 0);
            static void DestroyCommandPool(const uint32_t commandPoolId);
            static EntityCommandPool& GetEntityCommandPool(const uint32_t commandPoolId);


            static void ExecuteEntityCommands();


        private:

            static Table* GetTable(const Type archtype);
            static void SetEntityInfos(const uint32_t id, Table* table, const uint32_t batchIndex, const uint32_t rowIndex, const Type archtype);

            static void SortEntitiesToDestroy(std::vector<SlotInfo>& vector);
            
            static void ExecuteTransitionCommands(EntityCommandPool* entityCommandPool);
            static void ExecuteCreationCommands(EntityCommandPool* entityCommandPool);
            static void CompactBatches();

            static void UpdateQueries();

            static int32_t TryGetAvailableSlot(const Type archtype);

            inline static bool isInitialized = false;
            inline static std::unordered_map<Type, Table*> tables;
            inline static Entity* entities;
            inline static EntityRecord* entitiesRegister;
            inline static std::vector<EntityCommandPool> entityCommandPools;

            inline static bool updateQueries = true; 
            inline static std::vector<QueryInfo> queryInfos;
            inline static std::vector<std::vector<std::reference_wrapper<Table>>> queryResults;

            // Internal data
            inline static std::vector<SlotInfo> pendingDestructionEntities;
            inline static std::vector<SlotInfo> afterTransitionInvalidSlots;

            inline static std::vector<Type> newComponentsType;
            inline static std::vector<Type> oldComponentsType;
    };
}