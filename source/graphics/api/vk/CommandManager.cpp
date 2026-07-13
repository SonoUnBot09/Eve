#include <graphics/api/vk/ContextBuilder.hpp>
#include <graphics/api/vk/CommandManager.hpp>
#include <EveSettings.hpp>

CommandPoolHandle CommandManager::AllocateCommandPool()
{
    CommandPoolHandle handle;
    if(cmdPoolFreeSlots.empty())
    {
        handle.Id = cmdPools.size();
    }
    else 
    {
        handle.Id = cmdPoolFreeSlots.back();
        cmdPoolFreeSlots.pop_back();
    }

    for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandPoolCreateInfo cmdPoolCI
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = ContextBuilder::context.GraphicsQueueIndex
        };

        vkCreateCommandPool(ContextBuilder::context.Device, &cmdPoolCI, nullptr,
             &cmdPools[handle.Id + i]);
    }

    return handle;
}

CommandBufferHandle CommandManager::AllocateCommandBuffer(CommandPoolHandle pool)
{
    CommandBufferHandle handle;
    if(cmdBufferFreeSlots.empty())
    {
        handle.Id = cmdBuffers.size();
    }
    else 
    {
        handle.Id = cmdBufferFreeSlots.back();
        cmdBufferFreeSlots.pop_back();
    }

    for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandBufferAllocateInfo cmdBufferAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = cmdPools[pool.Id + i],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        vkAllocateCommandBuffers(ContextBuilder::context.Device, &cmdBufferAllocInfo, 
            &cmdBuffers[handle.Id + i]);
    }

    return handle;
}