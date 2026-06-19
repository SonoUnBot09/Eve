#pragma once

#include <unordered_map>

#include <Type.hpp>
#include <Entity.hpp>
#include <ComponentsRegistry.hpp>
#include <Table.hpp>
#include "EntityCommandPool.hpp"


class EntityManager
{
    public:

        static void RecordEntityCommandPool(EntityCommandPool& entityCommandPool);
        static void FreeAllMemory();

    private:
        
        static std::unordered_map<Type, Table> componentsPools;
        static Entity entities[10000];
        static EntityRecord entitiesRegister[10000];
        static std::vector<EntityCommandPool*> entityCommandPools;
};