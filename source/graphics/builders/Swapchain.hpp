#pragma once

#include <vulkan/vulkan.hpp>
#include <Eve/graphics/Texture.hpp>

namespace Eve::Graphics
{
    struct Swapchain
    {
        uint32_t Width, Height;
        VkSwapchainKHR Swapchain = nullptr;
        VkFormat Format;
        std::vector<TextureHandle> swapchainImagesHandles;
    };
}