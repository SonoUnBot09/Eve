#pragma once 

#include <graphics/api/vk/Context.hpp>
#include <graphics/api/vk/Swapchain.hpp>
#include <graphics/api/vk/Window.hpp>

namespace Eve::Graphics
{
    class SwapchainBuilder
    {
        public:
            static Swapchain Build(Context& context, Window& window, bool &success);
            static Swapchain Rebuild(Context& context, Window& window, bool &success);
        private:
        
            inline static VkFormat chooseFormat = VK_FORMAT_UNDEFINED;
            inline static uint32_t swapchainImagesCount = 0;
            inline static Swapchain swapchain;
    };
}