#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace Eve::Graphics
{
    struct Context
    {
        VkInstance Instance = nullptr;
        VkPhysicalDevice PhysicalDevice = nullptr;
        VkDevice Device = nullptr;
        VkSurfaceKHR Surface = nullptr;

        uint32_t GraphicsQueueIndex;
        VkQueue GraphicsQueue = nullptr;

        VmaAllocator Allocator = nullptr;

        struct PhysicalDeviceInfo
        {
            VkPhysicalDeviceMemoryProperties MemoryProperties;
            VkPhysicalDeviceProperties Properties;
            bool isDedicated;
        } PhysicalDeviceInfo;
    };
}