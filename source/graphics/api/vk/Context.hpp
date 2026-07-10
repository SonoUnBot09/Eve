#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
namespace Eve::Graphics
{
    struct Context
    {
        VkInstance Instance;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VkSurfaceKHR Surface;

        uint32_t GraphicsQueueIndex;
        VkQueue GraphicsQueue;

        VmaAllocator Allocator;
    };
}