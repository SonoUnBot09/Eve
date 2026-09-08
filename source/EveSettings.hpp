#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Eve::Settings
{
    inline static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;
    inline static constexpr uint32_t vulkanVersion {VK_API_VERSION_1_2};
    inline static constexpr bool useValidationLayers = true;
    inline static constexpr VkFormat swapchainFormats[] 
    {
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_FORMAT_B8G8R8A8_SRGB
    };
    inline static constexpr uint64_t transientTexturesStepPoolSize = 1024 * 1024 * 64;      // 64 MB
    inline static constexpr uint64_t transientBuffersStepPoolSize = 1024 * 1024 * 64;       // 64 MB
}