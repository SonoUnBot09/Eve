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

bool GraphicsCore::Render(uint64_t elapsedFrames, uint32_t frameIndex)
{
    if(isSwapchainRebuildNeeded)
    {
        vkDeviceWaitIdle(Context.Device);
        SwapchainBuilder::Rebuild(Swapchain);
        MemoryBin::DestroyAllPendingResources();
        isSwapchainRebuildNeeded = false;
    }

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

    RenderGraph::Execute(frameData.CmdBuffer, frameIndex, swaphchainImageIndex);


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