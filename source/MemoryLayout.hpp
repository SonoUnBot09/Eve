#pragma once

#include <cmath>
#include <iterator>
#include <unordered_map>

#include "Type.hpp"
#include "ComponentsRegistry.hpp"
#include "MemoryInfo.hpp"

struct MemoryLayout
{
    public:

        MemoryLayout(Type archtype, uint32_t batchSize)
        {

            const std::vector<Type> components = GetActiveComponentsType(archtype);


            uint32_t totalComponentsSize = 0;
            const std::vector<size_t> componentsStride = CalculateComponentsSize(components, &totalComponentsSize);
            
            uint32_t maxSingleComponentCount = (uint32_t)std::floor((float)batchSize / (float)totalComponentsSize);

            uint32_t offset = 0;
            for (uint32_t i = 0; i < components.size(); i++)
            {
                
                Type componentType = components[i];
                uint32_t componentSize = componentsStride[i];

                uint32_t componentOffset = componentSize * maxSingleComponentCount + offset;

                MemoryInfo memoryInfo(componentSize, componentOffset);
               
                componentsLayout[componentType] = memoryInfo;

                offset = componentOffset;

            }

            this->maxSingleComponentCount = maxSingleComponentCount;
        }

        MemoryInfo* GetMemoryInfo(const Type componentType);
        const uint32_t GetMaxSingleComponentsCountPerBatch();

    private:

        uint32_t maxSingleComponentCount;

        std::unordered_map<Type, MemoryInfo> componentsLayout;

        const std::vector<Type> GetActiveComponentsType(Type archtype);
        const std::vector<size_t> CalculateComponentsSize(const std::vector<Type>& components, uint32_t* totalComponentsSize);

};