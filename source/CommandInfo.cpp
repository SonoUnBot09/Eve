#include "EntityCommands.hpp"

void CommandInfo::RemoveComponent(const Type componentType)
{
    destructionComponentsType.push_back(componentType);
}

std::vector<std::byte>* CommandInfo::GetCreationComponents()
{
    return &creationComponents;
}

std::vector<Type>* CommandInfo::GetCreationComponentsType()
{
    return &creationComponentsType;
}

std::vector<Type>* CommandInfo::GetDestructionComponentsType()
{
    return &destructionComponentsType;
}