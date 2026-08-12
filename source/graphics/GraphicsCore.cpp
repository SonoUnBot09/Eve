#include "GraphicsCore.hpp"
#include "EveSettings.hpp"
#include "ResourceMapper.hpp"
#include "MemoryBin.hpp"
#include "builders/Context.hpp"
#include <graphics/RenderGraph.hpp>
#include <cstdint>

using namespace Eve::Graphics;

bool GraphicsCore::Initialize()
{
    if(!WindowBuilder::Build(Window))
    {
        return false;
    }

    if(!ContextBuilder::Build(Context))
    {
        return false;
    }

    if(!SwapchainBuilder::Build(Swapchain))
    {
        return false;
    }

    if(!FrameDataBuilder::Build(framesData, timelineSemaphore))
    {
        return false;
    }

    ResourceMapper::CreateGlobalDescriptor(1024, 8, 1024);

    return true;
}

bool GraphicsCore::Render(uint64_t elapsedFrames)
{
    if(isSwapchainRebuildNeeded)
    {
        vkDeviceWaitIdle(Context.Device);
        SwapchainBuilder::Rebuild(Swapchain);
        MemoryBin::DestroyAllPendingResources();
        isSwapchainRebuildNeeded = false;
    }

    uint32_t frameIndex = elapsedFrames % Eve::Settings::MAX_FRAMES_IN_FLIGHT;
    uint64_t timelineSignalValue = elapsedFrames + 1;
    uint64_t timelineWaitValue = timelineSignalValue - Eve::Settings::MAX_FRAMES_IN_FLIGHT;

    VkSemaphoreWaitInfo timelineWaitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pSemaphores = &timelineSemaphore,
        .pValues = &timelineWaitValue
    };

    vkWaitSemaphores(Context.Device, &timelineWaitInfo, UINT64_MAX);

    FrameData& frameData = framesData[frameIndex];

    vkResetCommandPool(Context.Device, frameData.CmdPool, 0);

    uint32_t swaphchainImageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(Context.Device, Swapchain.Swapchain, UINT64_MAX, 
        frameData.AcquiredImageSemaphore, nullptr, &swaphchainImageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        isSwapchainRebuildNeeded = true;
        return true;
    }
    else if(result == VK_SUBOPTIMAL_KHR)
    {
        isSwapchainRebuildNeeded = true;
    }

    // Compile Graph, Update Descriptor Set, Record Commands
    RenderGraph::Execute(frameData.CmdBuffer, frameIndex, swaphchainImageIndex);

    std::vector<VkSemaphoreSubmitInfo> waitSemaphores
    {
        VkSemaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frameData.AcquiredImageSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        }
    };

    std::vector<VkSemaphoreSubmitInfo> signalSemaphores
    {
        VkSemaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frameData.RenderCompletedSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        },
        VkSemaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = timelineSemaphore,
            .value = timelineSignalValue,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
        }
    };

    VkCommandBufferSubmitInfo cmdBufferSubmitInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = frameData.CmdBuffer
    };

    VkSubmitInfo2 submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,

        .waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphoreInfos = waitSemaphores.data(),

        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdBufferSubmitInfo,

        .signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphoreInfos = signalSemaphores.data()
    };

    vkQueueSubmit2(Context.GraphicsQueue, 1, &submitInfo, nullptr);

    VkPresentInfoKHR presentInfo
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frameData.RenderCompletedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &Swapchain.Swapchain,
        .pImageIndices = &swaphchainImageIndex
    };

    vkQueuePresentKHR(Context.GraphicsQueue, &presentInfo);

    MemoryBin::DestroyPendingResources();
}

void GraphicsCore::Destroy()
{
    if(!Context.Device) 
    {
        if(Window.Window)
        {
            SDL_DestroyWindow(Window.Window);
        }

        SDL_Quit();

        return;
    }
    
    vkDeviceWaitIdle(Context.Device);

    ResourceMapper::DestroyGlobalDescriptor();

    std::cout << "A" <<std::endl;
    SwapchainBuilder::Destroy(Swapchain);
    std::cout << "B" <<std::endl;
    MemoryBin::DestroyEverything();
    std::cout << "C" <<std::endl;

    if(Context.Allocator)
    {
        vmaDestroyAllocator(Context.Allocator);
    }

    if(Context.Surface)
    {
        vkDestroySurfaceKHR(Context.Instance, Context.Surface, nullptr);
    }

    if(Context.Device)
    {
        vkDestroyDevice(Context.Device, nullptr);
    }

    if(Context.Instance)
    {
        vkDestroyInstance(Context.Instance, nullptr);
    }

    volkFinalize();

    if(Window.Window)
    {
        SDL_DestroyWindow(Window.Window);
    }

    SDL_Quit();
}