#pragma once

#include <cstdint>

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

    Type createComponentsType;
    Type destroyComponentsType;

    EntityTransitionCommand(
        uint32_t entityId,
        uint32_t entityGeneration,
        Type createComponentsType,
        Type destroyComponentsType
    ) :
    entityId(entityId),
    entityGeneration(entityGeneration),
    createComponentsType(createComponentsType),
    destroyComponentsType(destroyComponentsType)
    {};
};