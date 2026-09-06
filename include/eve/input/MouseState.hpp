#pragma once

#include <glm/glm.hpp>

namespace Eve::Input
{
    struct MouseState
    {
        glm::vec2 MousePos;
        glm::vec2 MouseDelta;
    };
}