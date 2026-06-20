#include "EntityCommandPool.hpp"

void EntityCommandPool::ScheduleCreationCommand(const Entity entity, EntityCommandInfo* commandInfo)
{
    if(commandInfo != nullptr)
    {
        const Type archtype = commandInfo->GetCreateComponentsArchtype();

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

        uint32_t offset = 0;
        for (uint32_t i = 0; i < componentsCount; i++)
        {
            const size_t size = ComponentsRegistry::GetComponentSizeFromBit(componentsType[i]);

            componentsSize.push_back(size);
            componentsOffset.push_back(offset);

            offset += size;
        }

        const size_t availableSpace = creationComponentsData.size() - creationComponentsOffset;
        if(availableSpace < requiredSpace)
        {
            creationComponentsData.resize(creationComponentsData.size() + requiredSpace + 8192);
        }

        for (uint32_t i = 0; i < componentsCount; i++)
        {
            Type componentType = componentsType[i];
            uint32_t bitIndex = std::countr_zero(componentType.to_ullong());

            activeBits |= (1ULL << bitIndex);
            componentsIndices[bitIndex] = i;
        }

        for(uint32_t i = 0; i < 64; i++)
        {

            if((activeBits & (1ULL << i)) == 0) { continue; }

            uint32_t index = componentsIndices[i];
            
            const size_t size = componentsSize[index];

            std::memcpy
            (
                creationComponentsData.data() + creationComponentsOffset, 
                &components[componentsOffset[index]],
                size
            );

            creationComponentsOffset += size;
        }

        activeBits = 0;
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

    componentsSize.clear();
    componentsOffset.clear();
}

void EntityCommandPool::ScheduleDestructionCommand(const Entity entity)
{
    destructionCommands.emplace_back(
        entity.GenerataionId,
        entity.Id
    );
}

void EntityCommandPool::ScheduleTransitionCommand(const Entity entity, EntityCommandInfo& commandInfo)
{
    Type createComponentsArchtype = commandInfo.GetCreateComponentsArchtype();
    Type destroyComponentsArchtype = commandInfo.GetDestroyComponentsArchtype();

    transitionCommands.emplace_back(
        entity.Id,
        entity.GenerataionId,
        createComponentsArchtype,
        destroyComponentsArchtype,
        transitionComponentsOffset
    );

    const std::vector<std::byte>& components = commandInfo.GetCreationComponents();
    const std::vector<Type>& creationComponentsType = commandInfo.GetCreationComponentsType();

    const size_t requiredSpace = commandInfo.GetComponentsArraySize();
    const uint32_t componentsCount = creationComponentsType.size();

    uint32_t offset = 0;
    for (uint32_t i = 0; i < componentsCount; i++)
    {
        const size_t size = ComponentsRegistry::GetComponentSizeFromBit(creationComponentsType[i]);

        componentsSize.push_back(size);
        componentsOffset.push_back(offset);

        offset += size;
    }

    const size_t availableSpace = transitionComponentsData.size() - transitionComponentsOffset;
    if(availableSpace < requiredSpace)
    {
        transitionComponentsData.resize(transitionComponentsData.size() + requiredSpace + 8192);
    }

    for (uint32_t i = 0; i < componentsCount; i++)
    {
        Type componentType = creationComponentsType[i];
        uint32_t bitIndex = std::countr_zero(componentType.to_ullong());

        activeBits |= (1ULL << bitIndex);
        componentsIndices[bitIndex] = i;
    }

    for(uint32_t i = 0; i < 64; i++)
    {

        if((activeBits & (1ULL << i)) == 0) { continue; }

        uint32_t index = componentsIndices[i];
        
        const size_t size = componentsSize[index];

        std::memcpy
        (
            transitionComponentsData.data() + transitionComponentsOffset, 
            &components[componentsOffset[index]],
            size
        );

        transitionComponentsOffset += size;
    }

    activeBits = 0;

    componentsSize.clear();
    componentsOffset.clear();

}

void EntityCommandPool::Clear()
{
    creationCommands.clear();
    destructionCommands.clear();
    transitionCommands.clear();

    creationComponentsOffset = 0;
    transitionComponentsOffset = 0;
}