#pragma once

#include <vulkan/vulkan.hpp>

struct Swapchain
{
    uint32_t Width, Height;
    VkSwapchainKHR Swapchain;
    std::vector<VkImage> SwapchainImages;
    std::vector<VkImageView> SwapchainImageViews;
};