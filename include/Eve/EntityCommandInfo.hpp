#pragma once

#include <cstdint>
#include <vector>

#include <Eve/Type.hpp>

struct EntityCommandInfo
{

    EntityCommandInfo()
    {
        creationComponents.resize(2048);
        creationComponentsType.reserve(64);
    }
    EntityCommandInfo(
        uint32_t creationComponentsInitialSize, 
        uint32_t creationComponentsTypeInitialSize
    )
    {
        creationComponents.resize(creationComponentsInitialSize);
        creationComponentsType.reserve(creationComponentsTypeInitialSize);
    }

    public:

        template<typename T>
        void AddComponent(const T& component, const Type componentType)
        {
            uint32_t size = sizeof(component);
            uint32_t availableSpace = creationComponents.size() - creationComponentsOffset;
            if(availableSpace < size)
            {
                uint32_t baseSpace = creationComponents.size();
                creationComponents.resize(baseSpace + 512);
            }

            std::byte* dst = creationComponents.data() + creationComponentsOffset;
            std::memcpy(dst, &component, size);
            creationComponentsType.push_back(componentType);

            creationComponentsOffset += size;
            createComponentsArchtype = (createComponentsArchtype | componentType);

        }
        void RemoveComponent(const Type componentType);

        const Type GetCreateComponentsArchtype();
        const Type GetDestroyComponentsArchtype();
        const size_t GetComponentsArraySize();
        const std::vector<std::byte>& GetCreationComponents();
        const std::vector<Type>& GetCreationComponentsType();

    private:
        uint32_t creationComponentsOffset = 0;

        Type createComponentsArchtype = 0;
        Type destroyComponentsArchtype = 0;
        std::vector<std::byte> creationComponents;
        std::vector<Type> creationComponentsType;
};