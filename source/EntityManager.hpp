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


class EntityManager
{
    public:

        static Table* GetTable(const Type archtype);
        static void RecordEntityCommandPool(EntityCommandPool& entityCommandPool);
        static void FreeAllMemory();

    private:

        static void ExecuteEntityCommands();

        static void SortEntitiesToDestroy(std::vector<EntityInfo>& vector);
        static void ExecuteTransitionCommands(EntityCommandPool* entityCommandPool);
        static void ExecuteCreationCommands(EntityCommandPool* entityCommandPool);
        static void CompactBatches();

        static EntityInfo* TryGetAvailableSlot(const Type archtype);

        static std::unordered_map<Type, Table> componentsPools;
        static Entity entities[10000];
        static EntityRecord entitiesRegister[10000];
        static std::vector<EntityCommandPool*> entityCommandPools;


        static std::vector<EntityInfo> pendingDestructionEntities;
        static std::vector<EntityInfo> invalidEntities;
        static std::vector<Type> newComponentsType;
        static std::vector<Type> oldComponentsType;
};