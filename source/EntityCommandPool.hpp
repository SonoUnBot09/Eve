#pragma once

#include <vector>
#include "EntityCommands.hpp"
#include "Entity.hpp"

struct EntityCommandPool
{
    EntityCommandPool() = default;
    EntityCommandPool(
        uint32_t creationCommandBufferInitialSize, 
        uint32_t destructionCommandBufferInitialSize, 
        uint32_t transitionCommandBufferInitialSize,
        uint32_t creationComponentsInitialSize,
        uint32_t transitionComponentsInitialSize
    )
    {
        creationCommands.reserve(creationCommandBufferInitialSize);
        destructionCommands.reserve(destructionCommandBufferInitialSize);
        transitionCommands.reserve(transitionCommandBufferInitialSize);

        creationComponentsData.resize(creationComponentsInitialSize);
        transitionComponentsData.resize(transitionComponentsInitialSize);
    };

    public:

        void ScheduleCreationCommand(const Entity entity, EntityCommandInfo* commandInfo);
        void ScheduleDestructionCommand(const Entity entity);
        void ScheduleTransitionCommand(const Entity entity, EntityCommandInfo* commandInfo);

    private:

        std::vector<EntityCreationCommand> creationCommands;
        std::vector<EntityDestructionCommand> destructionCommands;
        std::vector<EntityTransitionCommand> transitionCommands;

        uint32_t creationComponentsOffset = 0;
        uint32_t transitionComponentsOffset = 0;

        std::vector<std::byte> creationComponentsData;
        std::vector<std::byte> transitionComponentsData;
};