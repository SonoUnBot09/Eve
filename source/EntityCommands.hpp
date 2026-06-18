#pragma once

#include <cstdint>
#include <vector>

#include "Type.hpp"

struct EntityCreationCommand
{
    uint32_t entityId;
    uint32_t entityGeneration;

    Type archtype;
    uint32_t componentOffset;

    EntityCreationCommand(
        uint32_t entityId, 
        uint32_t entityGeneration, 
        Type archtype,
        uint32_t componentOffset
    ) : 
    entityId(entityId),
    entityGeneration(entityGeneration),
    archtype(archtype),
    componentOffset(componentOffset)
    {};
};

struct EntityDestructionCommand
{
    uint32_t entityId;
    uint32_t entityGeneration;

    EntityDestructionCommand(
        uint32_t entityId,
        uint32_t entityGeneration
    ) :
    entityId(entityId),
    entityGeneration(entityGeneration)
    {};
};

struct EntityTransitionCommand
{
    uint32_t entityId;
    uint32_t entityGeneration;

    Type oldArchtype;
    Type newArchtype;

    EntityTransitionCommand(
        uint32_t entityId,
        uint32_t entityGeneration,
        Type oldArchtype,
        Type newArchtype
    ) :
    entityId(entityId),
    entityGeneration(entityGeneration),
    oldArchtype(oldArchtype),
    newArchtype(newArchtype)
    {};
};

struct EntityCommandInfo
{

    EntityCommandInfo()
    {
        creationComponents.resize(2048);
        creationComponentsType.reserve(64);
        destructionComponentsType.reserve(64);
    }
    EntityCommandInfo(
        uint32_t creationComponentsInitialSize, 
        uint32_t creationComponentsTypeInitialSize, 
        uint32_t destructionComponentsTypeInitialSize,
        Type currentArchtype
    ) : archtype(currentArchtype)
    {
        creationComponents.resize(creationComponentsInitialSize);
        creationComponentsType.reserve(creationComponentsTypeInitialSize);
        destructionComponentsType.reserve(destructionComponentsTypeInitialSize);
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
            archtype = (archtype | componentType);

        }
        void RemoveComponent(const Type componentType);

        const Type GetArchtype();
        const size_t GetComponentsArraySize();
        const std::vector<std::byte>& GetCreationComponents();
        const std::vector<Type>& GetCreationComponentsType();
        const std::vector<Type>& GetDestructionComponentsType();

    private:
        uint32_t creationComponentsOffset = 0;

        Type archtype = 0;
        std::vector<std::byte> creationComponents;
        std::vector<Type> creationComponentsType;
        std::vector<Type> destructionComponentsType;
};