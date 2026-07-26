#pragma once 

#include "Swapchain.hpp"

namespace Eve::Graphics
{
    class SwapchainBuilder
    {
        public:
            static bool Build(Swapchain& swapchin);
            static bool Rebuild(Swapchain& swapchin);
        private:
        
            inline static VkFormat chooseFormat = VK_FORMAT_UNDEFINED;
            inline static uint32_t swapchainImagesCount = 0;
    };
}