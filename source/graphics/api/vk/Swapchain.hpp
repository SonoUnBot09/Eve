#pragma once

#include <vulkan/vulkan.hpp>

struct Swapchain
{
    uint32_t Width, Height;
    VkSwapchainKHR Swapchain;
    VkFormat Format;
    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
};