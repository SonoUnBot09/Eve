#pragma once

#include <cmath>
#include <array>

#include <Eve/Type.hpp>
#include <Eve/ComponentsRegistry.hpp>
#include <Eve/MemoryInfo.hpp>

using namespace Eve::Entities;

namespace Eve::Internal
{
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

                    uint32_t componentOffset = offset;

                    MemoryInfo memoryInfo(componentSize, componentOffset);
                
                    //componentsLayout[componentType] = memoryInfo;
                    uint32_t index = std::countr_zero(componentType.to_ullong());
                    componentsLayout[index] = memoryInfo;
                    //componentsLayout.emplace(componentType, memoryInfo);
                    //componentsLayout.e

                    offset = componentOffset + componentSize * maxSingleComponentCount;

                }

                this->maxSingleComponentCount = maxSingleComponentCount;
            }

            MemoryInfo GetMemoryInfo(const Type componentType);
            const uint32_t GetMaxSingleComponentsCountPerBatch();

        private:

            uint32_t maxSingleComponentCount;

            std::array<MemoryInfo, 64> componentsLayout;

            const std::vector<Type> GetActiveComponentsType(Type archtype);
            const std::vector<size_t> CalculateComponentsSize(const std::vector<Type>& components, uint32_t* totalComponentsSize);

    };
}