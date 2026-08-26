#pragma once 

#include "Swapchain.hpp"
#include "graphics/builders/Swapchain.hpp"

namespace Eve::Graphics
{
    class SwapchainBuilder
    {
        public:
            static bool Build(Swapchain& swapchin);
            static bool Rebuild(Swapchain& swapchin);

            static void Destroy(Swapchain& swapchain);
        private:
        
            inline static VkFormat chooseFormat = VK_FORMAT_UNDEFINED;
    };
}