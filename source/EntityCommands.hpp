#pragma once

#include <cstdint>
#include <vector>

#include "Type.hpp"

struct EntityCreationCommand
{
    Type archtype;
    uint32_t componentOffset;
};

struct EntityDestructionCommand
{
    uint32_t entityID;
    uint32_t entityGeneration;
};

struct EntityTransitionCommand
{
    uint32_t entityID;
    uint32_t entityGeneration;

    Type oldArchtype;
    Type newArchtype;
};

struct CommandInfo
{

    CommandInfo()
    {
        creationComponents.reserve(2048);
        creationComponentsType.reserve(16);
        destructionComponentsType.reserve(16);
    }
    CommandInfo(
        uint32_t creationComponentsInitialSize, 
        uint32_t creationComponentsTypeInitialSize, 
        uint32_t destructionComponentsTypeInitialSize
    )
    {
        creationComponents.reserve(creationComponentsInitialSize);
        creationComponentsType.reserve(creationComponentsTypeInitialSize);
        destructionComponentsType.reserve(destructionComponentsTypeInitialSize);
    }

    public:

        template<typename T>
        void AddComponent(const T& component, const Type componentType)
        {
            uint32_t size = sizeof(component);
            uint32_t availableSpace = creationComponents.max_size() - creationComponentsOffset;
            if(availableSpace < size)
            {
                uint32_t baseSpace = creationComponents.max_size();
                creationComponents.resize(baseSpace + 512);
            }

            std::byte* dst = creationComponents.data() + creationComponentsOffset;
            std::memcpy(dst, &component, size);
            creationComponentsType.push_back(componentType);

            creationComponentsOffset += size;

        }
        void RemoveComponent(const Type componentType);

        std::vector<std::byte>* GetCreationComponents();
        std::vector<Type>* GetCreationComponentsType();
        std::vector<Type>* GetDestructionComponentsType();

    private:
        uint32_t creationComponentsOffset = 0;

        std::vector<std::byte> creationComponents;
        std::vector<Type> creationComponentsType;
        std::vector<Type> destructionComponentsType;
};