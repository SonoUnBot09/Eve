#pragma once

#include <SDL3/SDL.h>

struct Window
{
    SDL_Window* Window;
    uint32_t Width, Height;
};