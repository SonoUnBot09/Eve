#pragma once

#include <eve/math/Vector2.hpp>

namespace Eve::Input
{
    struct MouseState
    {
        Eve::Math::Vector2 MousePos;
        Eve::Math::Vector2 MouseDir;
    };
}