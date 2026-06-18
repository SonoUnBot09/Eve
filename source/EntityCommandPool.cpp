#include "EntityCommandPool.hpp"
#include "ComponentsRegistry.hpp"

void EntityCommandPool::ScheduleCreationCommand(const Entity entity, EntityCommandInfo* commandInfo)
{
    if(commandInfo != nullptr)
    {

        const Type archtype = commandInfo->GetArchtype();

        creationCommands.emplace_back(
            entity.Id,
            entity.GenerataionId,
            archtype,
            creationComponentsOffset
        );

        const std::vector<std::byte>& components = commandInfo->GetCreationComponents();
        const std::vector<Type>& componentsType = commandInfo->GetCreationComponentsType();

        const size_t requiredSpace = commandInfo->GetComponentsArraySize();
        const uint32_t componentsCount = componentsType.size();

        std::vector<size_t> componentsSize;
        std::vector<size_t> componentsOffset;
        componentsSize.reserve(componentsCount);
        componentsOffset.reserve(componentsCount);

        for (uint32_t i = 0; i < componentsCount; i++)
        {
            const size_t size = ComponentsRegistry::GetComponentSizeFromBit(componentsType[i]);
            componentsSize.push_back(size);
        }

        componentsOffset.push_back(0);
        for (uint32_t i = 1; i < componentsCount; i++)
        {
            const size_t offset = componentsSize[i - 1] + componentsOffset[i - 1];
            componentsOffset.push_back(offset);
        }

        const size_t availableSpace = creationComponentsData.size() - creationComponentsOffset;
        if(availableSpace < requiredSpace)
        {
            creationComponentsData.resize(creationComponentsData.size() + requiredSpace + 8192);
        }

        for (uint32_t i = 0; i < componentsCount; i++)
        {
            if(!archtype.test(i)) { continue; }

            for (uint32_t j = 0; j < componentsCount; j++)
            {
                if(!componentsType[j].test(i)) { continue; }

                const size_t size = componentsSize[j];

                std::memcpy(
                    creationComponentsData.data() + creationComponentsOffset, 
                    &components[componentsOffset[j]],
                    size
                );

                creationComponentsOffset += size;

            }
        }

    }
    else 
    {
        creationCommands.emplace_back(
            entity.Id,
            entity.GenerataionId,
            0,
            0
        );
    }
}

void EntityCommandPool::ScheduleDestructionCommand(const Entity entity)
{

}

void EntityCommandPool::ScheduleTransitionCommand(const Entity entity, EntityCommandInfo* commandInfo)
{

}