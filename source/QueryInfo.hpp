#pragma once

#include "Type.hpp"

struct QueryInfo
{
    Type ComponentsRequired;
    bool IsExclusive;

    QueryInfo() = default;
    QueryInfo(Type componentsRequired, bool isExclusive) :
    ComponentsRequired(componentsRequired),
    IsExclusive(isExclusive) {};
};