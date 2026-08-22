#pragma once

#include <vector>
#include <array>

#include <eve/entities/details/EntityCommands.hpp>
#include <eve/entities/EntityCommandInfo.hpp>
#include <eve/entities/ComponentsRegistry.hpp>
#include <eve/entities/Entity.hpp>

#include <eve/Debug.hpp>

namespace Eve::Entities
{
    class EntityCommandPool
    {

        public:

            EntityCommandPool()
            {
                creationCommands.reserve(defaultCreationCommandBufferSize);
                destructionCommands.reserve(defaultDestructionCommandBufferSize);
                transitionCommands.reserve(defaultTransitionCommandBufferSize);

                creationComponentsData.resize(defaultCreationComponentsSize);
                transitionComponentsData.resize(defaultTransitionComponentsSize);
                
                componentsSize.reserve(64);
                componentsOffset.reserve(64);
            }
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

            Entity ScheduleCreationCommand(EntityCommandInfo* commandInfo);
            void ScheduleDestructionCommand(const Entity entity);
            void ScheduleTransitionCommand(const Entity entity, EntityCommandInfo& commandInfo);

            void Clear();

            inline std::vector<EntityCreationCommand>& GetCreationCommands() { return creationCommands; }
            inline std::vector<EntityDestructionCommand>& GetDestructionCommands() { return destructionCommands; }
            inline std::vector<EntityTransitionCommand>& GetTransitionCommands() { return transitionCommands; }
            inline std::vector<std::byte>& GetCreationComponentsData() { return creationComponentsData; }
            inline std::vector<std::byte>& GetTransitionComponentsData() { return transitionComponentsData;}

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

            static constexpr uint32_t defaultCreationCommandBufferSize = 1000;
            static constexpr uint32_t defaultDestructionCommandBufferSize = 1000; 
            static constexpr uint32_t defaultTransitionCommandBufferSize = 1000;

            static constexpr uint32_t defaultCreationComponentsSize = 1024 * 64;     // 64 KB
            static constexpr uint32_t defaultTransitionComponentsSize = 1024 * 64;   // 64 KB
    };
}