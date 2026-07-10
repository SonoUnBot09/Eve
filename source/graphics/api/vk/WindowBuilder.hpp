#pragma once

#include <graphics/api/vk/Window.hpp>

class WindowBuilder
{
    public:
        static Window Build(bool& success);
        
    private:

        static bool InitializeSDLSubsystems();

        static bool CreateWindow();

        inline static bool isInitialized = false;
        inline static Window window;
};