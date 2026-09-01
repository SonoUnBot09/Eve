#pragma once

#include "graphics/builders/Window.hpp"
#include <graphics/builders/WindowBuilder.hpp>
#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/builders/SwapchainBuilder.hpp>
#include <graphics/builders/FrameDataBuilder.hpp>
#include <eve/math/Vector2Int.hpp>

using namespace Eve::Math;

namespace Eve::Graphics
{
    class GraphicsCore
    {
        public:

            static bool Initialize(std::vector<std::string>& searchShaderPaths);
            static void Destroy();

            static Vector2Int GetWindowSize() { return {static_cast<int32_t>(Window.Width), static_cast<int32_t>(Window.Height)}; }
            static bool Render(uint64_t elapsedFrames);
            
            inline static Window Window;
            inline static Context Context;
            inline static Swapchain Swapchain; 

            inline static uint32_t GetFrameIndex() { return frameIndex; }
        
        private:
            inline static bool isSwapchainRebuildNeeded = false;

            inline static std::vector<FrameData> framesData;
            inline static VkSemaphore timelineSemaphore;

            inline static uint32_t frameIndex = 0;
    };
}