#include "WindowBuilder.hpp"
#include "EveSettings.hpp"
#include "SDL3/SDL_video.h"

#include <SDL3/SDL.h>
#include <eve/debug/Debug.hpp>

using namespace Eve::Debug;
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
    SDL_WindowFlags flags = 0;

    window.Window = SDL_CreateWindow(Eve::Settings::InitialWindowTitle.c_str(), Eve::Settings::InitialWindowWidth, Eve::Settings::InitialWindowHeigth, SDL_WINDOW_VULKAN);
    window.Width = Eve::Settings::InitialWindowWidth;
    window.Height = Eve::Settings::InitialWindowHeigth;

    if(!window.Window)
    {
        return false;
    }

    SDL_SetWindowFullscreen(window.Window, Eve::Settings::WindowFullScreen);
    SDL_SetWindowResizable(window.Window, Eve::Settings::WindowResizable);
    SDL_SetWindowBordered(window.Window, Eve::Settings::WindowBordered);

    return true;
}
