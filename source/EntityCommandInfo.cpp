#include <Eve/EntityCommandInfo.hpp>
using namespace Eve::Entities;

void EntityCommandInfo::RemoveComponent(const Type componentType)
{
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

void EntityCommandInfo::Clean()
{
    creationComponentsOffset = 0;

    createComponentsArchtype = 0;
    destroyComponentsArchtype = 0;

    creationComponents.clear();
    creationComponentsType.clear();
}