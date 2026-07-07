#include <graphics/api/vk/WindowBuilder.hpp>

#include <SDL3/SDL.h>
#include <Eve/Debug.hpp>

using namespace Debug;

Window WindowBuilder::Build(bool& success)
{
    if(!isInitialized)
    {
        printError("SDL already initialized, cannot initialize it twice");
        success = false;
        return window;
    }

    if(!InitializeSDLSubsystems())
    {
        printError("Unable to initialize SDL subsystems");
        success = false;
        return window;
    }

    if(!CreateWindow())
    {
        printError("Unable to initialize SDL subsystems");
        success = false;
        return window;
    }

    success = true;
    return window;
}

bool WindowBuilder::InitializeSDLSubsystems()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        return false;
    }

    return true;
}

bool WindowBuilder::CreateWindow()
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
