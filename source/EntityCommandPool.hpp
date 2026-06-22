#pragma once

#include <vector>
#include <array>
#include <algorithm>

#include "EntityCommands.hpp"
#include "EntityCommandInfo.hpp"
#include "ComponentsRegistry.hpp"
#include "Entity.hpp"

class EntityCommandPool
{

    public:

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
            
            componentsSize.reserve(64);
            componentsOffset.reserve(64);
        };

        void ScheduleCreationCommand(const Entity entity, EntityCommandInfo* commandInfo);
        void ScheduleDestructionCommand(const Entity entity);
        void ScheduleTransitionCommand(const Entity entity, EntityCommandInfo& commandInfo);

        inline std::vector<EntityCreationCommand>& GetCreationCommands()
        {
            return creationCommands;
        }
        inline std::vector<EntityDestructionCommand>& GetDestructionCommands()
        {
            return destructionCommands;
        }
        inline std::vector<EntityTransitionCommand>& GetTransitionCommands()
        {
            return transitionCommands;
        }

        inline std::vector<std::byte>& GetCreationComponentsData()
        {
            return creationComponentsData; 
        }
        inline std::vector<std::byte>& GetTransitionComponentsData()
        {
            return transitionComponentsData;
        }

        void Clear();

    private:

        std::vector<EntityCreationCommand> creationCommands;
        std::vector<EntityDestructionCommand> destructionCommands;
        std::vector<EntityTransitionCommand> transitionCommands;

        uint32_t creationComponentsOffset = 0;
        uint32_t transitionComponentsOffset = 0;

        std::vector<std::byte> creationComponentsData;
        std::vector<std::byte> transitionComponentsData;

        // Stuff needed for the internal functioning
        std::vector<size_t> componentsSize;
        std::vector<size_t> componentsOffset;

        uint64_t activeBits = 0;
        std::array<uint32_t, 64> componentsIndices = {0};
};