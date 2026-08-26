#include <graphics/GraphicsCore.hpp>
#include <graphics/CommandManager.hpp>
#include <EveSettings.hpp>
#include <graphics/ErrorManager.hpp>

using namespace Eve::Graphics;

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

        VK_CHECK(vkAllocateCommandBuffers(GraphicsCore::Context.Device, &cmdBufferAllocInfo, 
            &cmdBuffers[handle.Id + i]));
    }

    return handle;
}