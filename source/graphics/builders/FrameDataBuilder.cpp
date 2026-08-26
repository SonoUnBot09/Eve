#include <graphics/GraphicsCore.hpp>
#include "FrameDataBuilder.hpp"
#include "EveSettings.hpp"
#include <graphics/ErrorManager.hpp>

using namespace Eve::Graphics;

bool FrameDataBuilder::Build(std::vector<FrameData>& frameData, VkSemaphore& timelineSemaphore)
{
    frameData.resize(Eve::Settings::MAX_FRAMES_IN_FLIGHT);

    for(uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandPoolCreateInfo cmdPoolCI
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = GraphicsCore::Context.GraphicsQueueIndex
        };

        VK_CHECK(vkCreateCommandPool(GraphicsCore::Context.Device, &cmdPoolCI, nullptr, &frameData[i].CmdPool));

        VkCommandBufferAllocateInfo cmdBufferAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = frameData[i].CmdPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VK_CHECK(vkAllocateCommandBuffers(GraphicsCore::Context.Device, &cmdBufferAllocInfo, &frameData[i].CmdBuffer));

        VkSemaphoreCreateInfo semaphoreCI
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        VK_CHECK(vkCreateSemaphore(GraphicsCore::Context.Device, &semaphoreCI, nullptr,
            &frameData[i].AcquiredImageSemaphore));

        VK_CHECK(vkCreateSemaphore(GraphicsCore::Context.Device, &semaphoreCI, nullptr, 
            &frameData[i].RenderCompletedSemaphore));
    }

    VkSemaphoreTypeCreateInfo semaphoreTypeCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    VkSemaphoreCreateInfo semaphoreCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaphoreTypeCI
    };

    VK_CHECK(vkCreateSemaphore(GraphicsCore::Context.Device, & semaphoreCI, nullptr, &timelineSemaphore));

    return true;
}