#pragma once

#include <SDL3/SDL.h>

namespace Eve::Graphics
{
    struct Window
    {
        SDL_Window* Window;
        uint32_t Width, Height;
    };
}