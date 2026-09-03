#include "GraphicsCore.hpp"
#include "EveSettings.hpp"
#include "ResourceMapper.hpp"
#include "MemoryBin.hpp"
#include "builders/Context.hpp"
#include "builders/FrameData.hpp"
#include "graphics/builders/PipelineBuilder.hpp"
#include "registers/ShaderRegistry.hpp"
#include <graphics/RenderGraph.hpp>
#include <cstdint>
#include <graphics/ErrorManager.hpp>

using namespace Eve::Graphics;

bool GraphicsCore::Initialize(std::vector<std::string>& searchShaderPaths)
{
    if(!WindowBuilder::Build(Window))
    {
        return false;
    }

    if(!ContextBuilder::Build(Context))
    {
        return false;
    }

    ResourceMapper::CreateGlobalDescriptor(1024, 8, 1024);

    SlangCompiler::Initialize(searchShaderPaths);
    
    PipelineBuilder::Initialize();

    if(!SwapchainBuilder::Build(Swapchain))
    {
        return false;
    }

    if(!FrameDataBuilder::Build(framesData, timelineSemaphore))
    {
        return false;
    }

    return true;
}

bool GraphicsCore::Render(uint64_t elapsedFrames)
{
    if(isSwapchainRebuildNeeded)
    {
        VK_CHECK(vkDeviceWaitIdle(Context.Device));
        SwapchainBuilder::Rebuild(Swapchain);
        MemoryBin::DestroyAllPendingResources();
        isSwapchainRebuildNeeded = false;
    }

    uint64_t timelineSignalValue = elapsedFrames + 1;
    uint64_t timelineWaitValue = timelineSignalValue - Eve::Settings::MAX_FRAMES_IN_FLIGHT;

    VkSemaphoreWaitInfo timelineWaitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &timelineSemaphore,
        .pValues = &timelineWaitValue
    };

    VK_CHECK(vkWaitSemaphores(Context.Device, &timelineWaitInfo, UINT64_MAX));

    FrameData& frameData = framesData[frameIndex];

    VK_CHECK(vkResetCommandPool(Context.Device, frameData.CmdPool, 0));

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
    else 
    {
        VK_CHECK(result);
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

    VkSubmitInfo2KHR submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,

        .waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphores.size()),
        .pWaitSemaphoreInfos = waitSemaphores.data(),

        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdBufferSubmitInfo,

        .signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphores.size()),
        .pSignalSemaphoreInfos = signalSemaphores.data()
    };

    VK_CHECK(vkQueueSubmit2KHR(Context.GraphicsQueue, 1, &submitInfo, nullptr));

    VkPresentInfoKHR presentInfo
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frameData.RenderCompletedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &Swapchain.Swapchain,
        .pImageIndices = &swaphchainImageIndex
    };

    VK_CHECK(vkQueuePresentKHR(Context.GraphicsQueue, &presentInfo));

    MemoryBin::DestroyPendingResources();

    return true;
}

void GraphicsCore::Destroy()
{
    if(Context.Device) 
    {
        VK_CHECK(vkDeviceWaitIdle(Context.Device));
    }

    for(uint32_t i = 0; i < framesData.size(); i++)
    {
        FrameData& frameData = framesData[i];

        vkDestroyCommandPool(Context.Device, frameData.CmdPool, nullptr);

        vkDestroySemaphore(Context.Device, frameData.AcquiredImageSemaphore, nullptr);
        vkDestroySemaphore(Context.Device, frameData.RenderCompletedSemaphore, nullptr);
    }

    vkDestroySemaphore(Context.Device, timelineSemaphore, nullptr);

    ShaderRegistry::DestroyAllShaders();

    ResourceMapper::DestroyGlobalDescriptor();

    SwapchainBuilder::Destroy(Swapchain);

    MemoryBin::DestroyEverything();

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