#pragma once

#include <vulkan/vulkan.hpp>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/ShaderHandle.hpp>

namespace Eve::Graphics
{
    struct Swapchain
    {
        uint32_t Width, Height;
        VkSwapchainKHR Swapchain = nullptr;
        VkFormat Format;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        ShaderHandle shader;
    };
}