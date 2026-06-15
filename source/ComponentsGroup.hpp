#pragma once

#include <cstdint>

#include <Type.hpp>
#include <ComponentsRegistry.hpp>

class ComponentsGroup
{
    public:

        ComponentsGroup(Type componentsType, uint32_t maxComponentsCountPerBatch)
        {
            this->maxComponentsCountPerBatch = maxComponentsCountPerBatch;
            this->componentSize = ComponentsRegistry::GetComponentSizeFromBit(componentsType);
        }
        
        char* GetComponentsBatchAddress(uint32_t batchIndex)
        {
            return componentsBatch[batchIndex];
        }
        
        uint32_t* GetEntitiesBatchAddress(uint32_t batchIndex)
        {
            return entitiesIndex[batchIndex];
        }

        void AllocateBatch(uint32_t batchCount)
        {
            for (uint32_t i = 0; i < batchCount; i++)
            {
                char* componentsArray = new char[componentSize * maxComponentsCountPerBatch];
                uint32_t* entitiesArray = new uint32_t[maxComponentsCountPerBatch];

                componentsBatch.push_back(componentsArray);
                entitiesIndex.push_back(entitiesArray);
            }
        }

        ~ComponentsGroup()
        {
            for (char* &batch : componentsBatch)
            {
                if(batch != nullptr)
                {
                    delete[] batch;
                }
            }

            for (uint32_t* &batch : entitiesIndex)
            {
                if(batch != nullptr)
                {
                    delete[] batch;
                }
            }
        }

    private:

        std::vector<char*> componentsBatch;
        std::vector<uint32_t*> entitiesIndex;

        uint32_t maxComponentsCountPerBatch;
        size_t componentSize;
};