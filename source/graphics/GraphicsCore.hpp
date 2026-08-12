#pragma once

#include <graphics/builders/WindowBuilder.hpp>
#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/builders/SwapchainBuilder.hpp>
#include <graphics/builders/FrameDataBuilder.hpp>

namespace Eve::Graphics
{
    class GraphicsCore
    {
        public:

            static bool Initialize();
            static void Destroy();

            static bool Render(uint64_t elapsedFrames);
            
            inline static Window Window;
            inline static Context Context;
            inline static Swapchain Swapchain; 
        
        private:
            inline static bool isSwapchainRebuildNeeded = false;

            inline static std::vector<FrameData> framesData;
            inline static VkSemaphore timelineSemaphore;
    };
}