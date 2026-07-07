#pragma once

#include <graphics/api/vk/Window.hpp>

class WindowBuilder
{
    public:
        Window Build(bool& success);
        
    private:

        bool InitializeSDLSubsystems();

        bool CreateWindow();

        inline static bool isInitialized = false;
        Window window;
};