#pragma once

#include <unordered_map>

#include <Type.hpp>
#include <Entity.hpp>
#include <ComponentsRegistry.hpp>
#include <ArchtypeComponents.hpp>


class EntityManager
{
    public:

        static void FreeAllMemory()
        {
            componentsPools.clear();
        }

    private:
        
        static std::unordered_map<Type, ArchtypeComponents> componentsPools;
        static Entity entities[10000];
        static EntityRecord entitiesRegister[10000];
};