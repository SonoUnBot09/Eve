#pragma once

#include "ComponentsGroup.hpp"
#include <unordered_map>
#include <cstdint>

class ArchtypeComponents
{
    public:
        ArchtypeComponents(Type archtype, uint32_t maxComponentsPerBatch)
        {
            this->archtype = archtype;
            this->maxComponentsPerBatch = maxComponentsPerBatch;

            for (int i = 0; i < 64; i++)
            {
                if(!archtype.test(i)) { continue; }
                
                Type componentType = 0;
                componentType.set(i);

                ComponentsGroup* group = new ComponentsGroup(componentType, maxComponentsPerBatch);
    
                groups[componentType] = *group;
            }
        }

        ComponentsGroup* GetComponentsGroup(Type componentType)
        {
            auto group = groups.find(componentType);

            if(group == groups.end())
            {
                return nullptr;
            }

            return &(group->second);
        }

        ~ArchtypeComponents()
        {
            for (int i = 0; i < 64; i++)
            {
                if(!archtype.test(i)) { continue; }
                
                Type componentType = 0;
                componentType.set(i);

                ComponentsGroup* group = GetComponentsGroup(componentType);

                if(group == nullptr) { continue; }

                delete group;
            }
        }

    private:
        Type archtype;
        uint32_t maxComponentsPerBatch;

        std::unordered_map<Type, ComponentsGroup> groups;
        std::list<uint32_t> componentsPerBatch;
};