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

#include "iostream"


class EntityManager
{
    public:

        static void RecordEntityCommandPool(EntityCommandPool& entityCommandPool);
        static void ExecuteEntityCommands();
        static void FreeAllMemory();

    private:

        static Table* GetTable(const Type archtype);
        static void SetEntityInfos(const uint32_t id, Table* table, const uint32_t batchIndex, const uint32_t rowIndex, const Type archtype);

        static void SortEntitiesToDestroy(std::vector<SlotInfo>& vector);
        
        static void ExecuteTransitionCommands(EntityCommandPool* entityCommandPool);
        static void ExecuteCreationCommands(EntityCommandPool* entityCommandPool);
        static void CompactBatches();

        static SlotInfo* TryGetAvailableSlot(const Type archtype);

        inline static std::unordered_map<Type, Table> componentsPools;
        inline static Entity* entities;
        inline static EntityRecord* entitiesRegister;
        inline static std::vector<EntityCommandPool*> entityCommandPools;


        inline static std::vector<SlotInfo> pendingDestructionEntities;
        inline static std::vector<SlotInfo> afterTransitionInvalidSlots;

        inline static std::vector<Type> newComponentsType;
        inline static std::vector<Type> oldComponentsType;
};