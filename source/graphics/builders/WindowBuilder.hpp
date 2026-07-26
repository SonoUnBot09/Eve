#pragma once

#include "Window.hpp"

namespace Eve::Graphics
{
    class WindowBuilder
    {
        public:
            static bool Build(Window& window);
            
        private:

            static bool InitializeSDLSubsystems();

            static bool CreateWindow(Window& window);
    };
}