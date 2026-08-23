#pragma once

#include <cmath>
#include <array>

#include <eve/entities/Type.hpp>
#include <eve/entities/ComponentsRegistry.hpp>
#include <eve/entities/details/MemoryInfo.hpp>

using namespace Eve::Entities;

struct MemoryLayout
{
    public:

        MemoryLayout() = default;
        MemoryLayout(Type archtype, uint32_t batchSize)
        {
            const std::vector<Type> componentTypes = GetActiveComponentsType(archtype);
            
            activeComponentsTypes = componentTypes;

            uint32_t totalComponentsSize = 0;
            const std::vector<size_t> componentSizes = CalculateComponentSizes(componentTypes, totalComponentsSize);
            
            uint32_t maxEntitiesCount = (uint32_t)std::floor((float)batchSize / (float)totalComponentsSize);

            // Calculate components strides
            uint32_t offset = 0;
            for (uint32_t i = 0; i < componentTypes.size(); i++)
            {
                Type componentType = componentTypes[i];
                uint32_t componentSize = componentSizes[i];

                MemoryInfo memoryInfo(componentSize, offset);
            
                uint32_t index = std::countr_zero(componentType.to_ullong());
                componentMemoryInfos[index] = memoryInfo;

                offset =+ componentSize * maxEntitiesCount;
            }

            this->maxEntitiesCount = maxEntitiesCount;
        }

        MemoryInfo GetMemoryInfo(const Type componentType);
        inline const std::vector<Type> GetActiveComponentsTypes() { return activeComponentsTypes; }
        const uint32_t GetMaxEntityCountPerBatch();

    private:

        uint32_t maxEntitiesCount;

        std::array<MemoryInfo, 64> componentMemoryInfos;
        std::vector<Type> activeComponentsTypes;

        const std::vector<Type> GetActiveComponentsType(Type archtype);
        const std::vector<size_t> CalculateComponentSizes(const std::vector<Type>& components, uint32_t& totalComponentsSize);

};