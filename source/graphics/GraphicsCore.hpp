#pragma once

#include <graphics/builders/WindowBuilder.hpp>
#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/builders/SwapchainBuilder.hpp>

namespace Eve::Graphics
{
    class GraphicsCore
    {
        public:
            inline static Window Window;
            inline static Context Context;
            inline static Swapchain Swapchain; 
    };
}