#pragma once

#include <graphics/builders/ContextBuilder.hpp>
#include "graphics/builders/Window.hpp"
#include <graphics/builders/WindowBuilder.hpp>
#include <graphics/builders/SwapchainBuilder.hpp>
#include <graphics/builders/FrameDataBuilder.hpp>
#include <glm/glm.hpp>
#include <EveSettings.hpp>

namespace Eve
{
    class Core;
}


namespace Eve::Graphics
{
    class GraphicsCore
    {
        public:

            static bool Initialize(std::vector<std::string>& searchShaderPaths);
            static void Destroy();

            static glm::ivec2 GetWindowSize() { return glm::ivec2(static_cast<int32_t>(Window.Width), static_cast<int32_t>(Window.Height)); }
            static bool Render();
            
            inline static Window Window;
            inline static Context Context;
            inline static Swapchain Swapchain; 

            inline static uint32_t GetFrameIndex() { return frameIndex; }
            inline static bool CanRenderOnSwapchain() { return canRenderOnSwapchain; }
        
        private:
            inline static bool isSwapchainRebuildNeeded = false;
            inline static bool canRenderOnSwapchain = true;

            inline static std::vector<FrameData> framesData;
            inline static VkSemaphore timelineSemaphore;

            inline static uint32_t frameIndex = 0;
            inline static uint64_t elapsedGraphicsFrames = Eve::Settings::MAX_FRAMES_IN_FLIGHT;

            friend class Eve::Core;
    };
}