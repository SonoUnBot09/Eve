#include "EntityCommands.hpp"

void EntityCommandInfo::RemoveComponent(const Type componentType)
{
    destructionComponentsType.push_back(componentType);

    destroyComponentsArchtype = (destroyComponentsArchtype | componentType);
}

const Type EntityCommandInfo::GetCreateComponentsArchtype()
{
    return createComponentsArchtype;
}

const Type EntityCommandInfo::GetDestroyComponentsArchtype()
{
    return destroyComponentsArchtype;
}

const size_t EntityCommandInfo::GetComponentsArraySize()
{
    return creationComponentsOffset;
}

const std::vector<std::byte>& EntityCommandInfo::GetCreationComponents()
{
    return creationComponents;
}

const std::vector<Type>& EntityCommandInfo::GetCreationComponentsType()
{
    return creationComponentsType;
}

const std::vector<Type>& EntityCommandInfo::GetDestructionComponentsType()
{
    return destructionComponentsType;
}