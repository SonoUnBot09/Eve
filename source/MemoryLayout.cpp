#include "MemoryLayout.hpp"
#include <cstddef>

const std::vector<Type> MemoryLayout::GetActiveComponentsType(Type archtype)
{
    std::vector<Type> componets;
    for (uint32_t i = 0; i < 64; i++)
    {
        if(!archtype.test(i)) { continue; }

        Type componentType = 0;
        componentType.set(i);

        componets.push_back(componentType);
    }

    return componets;
}

const std::vector<size_t> MemoryLayout::CalculateComponentsSize(const std::vector<Type>& components, uint32_t* totalComponentsSize)
{
    std::vector<size_t> componentsStride;
    for (uint32_t i = 0; i < components.size(); i++)
    {
        Type componentType = components[i];

        size_t size = ComponentsRegistry::GetComponentSizeFromBit(componentType);

        componentsStride.push_back(size);
        *totalComponentsSize += size;
    }

    return componentsStride;
}

MemoryInfo* MemoryLayout::GetMemoryInfo(const Type componentType)
{
    auto tuple = componentsLayout.find(componentType);

    if(tuple == componentsLayout.end()) { return nullptr; }

    return &tuple->second;
}