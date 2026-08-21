#include "WindowBuilder.hpp"

#include <SDL3/SDL.h>
#include <eve/Debug.hpp>

using namespace Debug;
using namespace Eve::Graphics;

bool WindowBuilder::Build(Window& window)
{
    if(!InitializeSDLSubsystems())
    {
        printError("Unable to initialize SDL subsystems");
        return false;
    }

    if(!CreateWindow(window))
    {
        printError("Unable to initialize SDL subsystems");
        return false;
    }

    return true;
}

bool WindowBuilder::InitializeSDLSubsystems()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        return false;
    }

    return true;
}

bool WindowBuilder::CreateWindow(Window& window)
{
    window.Window = SDL_CreateWindow("Eve", 512, 512, SDL_WINDOW_RESIZABLE);
    window.Height = 512;
    window.Width = 512;

    if(!window.Window)
    {
        return false;
    }

    return true;
}
