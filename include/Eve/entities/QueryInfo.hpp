#pragma once

#include <Eve/Entities/Type.hpp>

namespace Eve::Entities
{
    struct QueryInfo
    {
        Type ComponentsRequired;
        bool IsExclusive;

        QueryInfo() = default;
        QueryInfo(Type componentsRequired, bool isExclusive) :
        ComponentsRequired(componentsRequired),
        IsExclusive(isExclusive) {};
    };
}