#pragma once

#include <graphics/builders/WindowBuilder.hpp>
#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/builders/SwapchainBuilder.hpp>

namespace Eve::Graphics
{
    class GraphicsCore
    {
        public:

            static bool Initialize();
            static void Destroy();
            
            inline static Window Window;
            inline static Context Context;
            inline static Swapchain Swapchain; 
    };
}