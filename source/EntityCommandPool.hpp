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

        creationComponentsData.reserve(creationComponentsInitialSize);
        transitionComponentsData.reserve(transitionComponentsInitialSize);
    };

    public:

        void ScheduleCreationCommand(const Entity entity, const CommandInfo* commandInfo);
        void ScheduleDestructionCommand(const Entity entity);
        void ScheduleTransitionCommand(const Entity entity, const CommandInfo* commandInfo);

    private:

        std::vector<EntityCreationCommand> creationCommands;
        std::vector<EntityDestructionCommand> destructionCommands;
        std::vector<EntityTransitionCommand> transitionCommands;

        std::vector<char> creationComponentsData;
        std::vector<char> transitionComponentsData;
};