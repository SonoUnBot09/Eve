#pragma once 

#include <graphics/api/vk/Context.hpp>
#include <graphics/api/vk/Swapchain.hpp>
#include <graphics/api/vk/Window.hpp>

class SwapchainBuilder
{
    public:
        static Swapchain Build(Context& context, Window& window, bool &success);
        static Swapchain Rebuild(Context& context, Window& window, bool &success);
    private:

        static constexpr VkFormat swapchainFormats[] 
        {
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_FORMAT_B8G8R8A8_SRGB
        };
        inline static VkFormat chooseFormat = VK_FORMAT_UNDEFINED;
        inline static uint32_t swapchainImagesCount = 0;
        inline static Swapchain swapchain;
};