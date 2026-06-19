#include "EntityManager.hpp"


void EntityManager::RecordEntityCommandPool(EntityCommandPool& entityCommandPool)
{
    entityCommandPools.push_back(&entityCommandPool);
}

void EntityManager::FreeAllMemory()
{
    componentsPools.clear();
}