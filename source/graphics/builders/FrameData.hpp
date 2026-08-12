#pragma once

#include <vulkan/vulkan.hpp>

struct FrameData
{
    VkCommandPool CmdPool;
    VkCommandBuffer CmdBuffer;

    VkSemaphore AcquiredImageSemaphore;
    VkSemaphore RenderCompletedSemaphore;
};