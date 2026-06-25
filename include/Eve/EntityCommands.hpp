#pragma once

#include <cstdint>

#include <Eve/Type.hpp>

struct EntityCreationCommand
{
    uint32_t Id;
    uint32_t GenerationId;

    Type Archtype;
    uint32_t ComponentOffset;

    EntityCreationCommand(
        uint32_t id, 
        uint32_t generationId, 
        Type archtype,
        uint32_t componentOffset
    ) : 
    Id(id),
    GenerationId(generationId),
    Archtype(archtype),
    ComponentOffset(componentOffset)
    {};
};

struct EntityDestructionCommand
{
    uint32_t Id;
    uint32_t GenerationId;

    EntityDestructionCommand(
        uint32_t id,
        uint32_t generationId
    ) :
    Id(id),
    GenerationId(generationId)
    {};
};

struct EntityTransitionCommand
{
    uint32_t Id;
    uint32_t GenerationId;

    Type CreateComponentsType;
    Type DestroyComponentsType;

    uint32_t ComponentOffset = 0;

    EntityTransitionCommand(
        uint32_t id,
        uint32_t generationId,
        Type createComponentsType,
        Type destroyComponentsType,
        uint32_t componentsOffset
    ) :
    Id(id),
    GenerationId(generationId),
    CreateComponentsType(createComponentsType),
    DestroyComponentsType(destroyComponentsType),
    ComponentOffset(componentsOffset)
    {};
};