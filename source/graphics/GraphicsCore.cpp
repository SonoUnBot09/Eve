#include "GraphicsCore.hpp"
#include "ResourceMapper.hpp"
#include "MemoryBin.hpp"
#include "builders/SwapchainBuilder.hpp"

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

    ResourceMapper::CreateGlobalDescriptor(1024, 8, 1024);

    return true;
}

void GraphicsCore::Destroy()
{
    vkDeviceWaitIdle(Context.Device);

    ResourceMapper::DestroyGlobalDescriptor();

    MemoryBin::DestroyEverythingNow();

    SwapchainBuilder::Destroy(Swapchain);

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