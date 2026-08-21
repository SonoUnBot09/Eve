#pragma once

#include <vulkan/vulkan.hpp>
#include <eve/graphics/Texture.hpp>
#include <eve/graphics/ShaderHandle.hpp>
#include <eve/graphics/Sampler.hpp>

namespace Eve::Graphics
{
    struct Swapchain
    {
        uint32_t Width, Height;
        VkSwapchainKHR Swapchain = nullptr;
        VkFormat Format;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        SamplerHandle sampler;
        ShaderHandle shader;
    };
}