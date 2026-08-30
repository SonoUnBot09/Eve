#pragma once

#include <cstdint>

namespace Eve::Input
{
    enum class MouseKey : uint16_t
    {
        BUTTON_LEFT = 1, BUTTON_RIGHT, WHEEL
    };
}