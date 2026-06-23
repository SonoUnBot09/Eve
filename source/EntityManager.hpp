#pragma once

#include <unordered_map>
#include <vector>
#include <algorithm>

#include <Type.hpp>
#include <Entity.hpp>
#include <ComponentsRegistry.hpp>
#include <Table.hpp>
#include "EntityCommandPool.hpp"
#include "EntityCommands.hpp"
#include "QueryInfo.hpp"

#include "iostream"

class EntityManager
{
    public:
        //10000, 500
        static void Initialize(const uint32_t maxEntityCount = 1, const uint32_t maxEntityCommandsCount = 1);
        static void Destroy();

        static std::vector<Table*>& GetTablesFromQuery(const uint32_t queryTicket);

        static uint32_t RegisterQuery(const QueryInfo queryInfo);
        static void RegisterEntityCommandPool(EntityCommandPool& entityCommandPool);
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
        inline static std::vector<EntityCommandPool*> entityCommandPools;

        inline static bool updateQueries = true; 
        inline static std::vector<QueryInfo> queryInfos;
        inline static std::vector<std::vector<Table*>> queryResults;

        // Internal data
        inline static std::vector<SlotInfo> pendingDestructionEntities;
        inline static std::vector<SlotInfo> afterTransitionInvalidSlots;

        inline static std::vector<Type> newComponentsType;
        inline static std::vector<Type> oldComponentsType;
};