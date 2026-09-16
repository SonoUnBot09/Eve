#include <filesystem>
#include <graphics/GraphicsCore.hpp>
#include "EveSettings.hpp"
#include "SDL3/SDL_video.h"
#include <eve/window/Window.hpp>
#include <SDL3/SDL.h>
#include <ExecutablePath.hpp>

using namespace Eve::Window;

void Window::SetFullscreen(bool isFullscreen)
{
    if(Eve::Settings::WindowFullScreen != isFullscreen)
    {
        SDL_SetWindowFullscreen(Eve::Graphics::GraphicsCore::Window.Window, isFullscreen);
        Eve::Settings::WindowFullScreen = isFullscreen;
    }
}

void Window::SetBordered(bool isBordered)
{
    if(Eve::Settings::WindowBordered != isBordered)
    {
        SDL_SetWindowBordered(Eve::Graphics::GraphicsCore::Window.Window, isBordered);
        Eve::Settings::WindowBordered = isBordered;
    }
}

void Window::SetResizable(bool isResizable)
{
    if(Eve::Settings::WindowResizable != isResizable)
    {
        SDL_SetWindowResizable(Eve::Graphics::GraphicsCore::Window.Window, isResizable);
        Eve::Settings::WindowResizable = isResizable;
    }
}

void Window::SetResolution(uint32_t width, uint32_t height)
{
    SDL_SetWindowSize(Eve::Graphics::GraphicsCore::Window.Window, width, height);
}

void Window::SetPosition(uint32_t x, uint32_t y)
{
    SDL_SetWindowPosition(Eve::Graphics::GraphicsCore::Window.Window, x, y);
}

void Window::SetPositionAtCenter()
{
    SDL_SetWindowPosition(Eve::Graphics::GraphicsCore::Window.Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window::SetWindowTitle(std::string title)
{
    SDL_SetWindowTitle(Eve::Graphics::GraphicsCore::Window.Window, title.c_str());
}

glm::ivec2 Window::GetWindowSize()
{
    return Eve::Graphics::GraphicsCore::GetWindowSize();
}

bool Window::SetIcon(std::string path)
{
    std::string iconPath = GetExecutableDirectory().string() + "\\" + path;

    SDL_Surface* icon = SDL_LoadPNG(iconPath.c_str());

    if(icon)
    {
        std::cout << "Icon found" << std::endl;
        
        bool success = SDL_SetWindowIcon(Eve::Graphics::GraphicsCore::Window.Window, icon);

        SDL_DestroySurface(icon);

        return success;
    }

    std::cout << "Icon not found | " << "Path: " << iconPath << std::endl;

    return false;
}