#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <Volk/volk.h>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <iostream>

class Application
{

    public:

        static constexpr uint32_t vulkanVersion {VK_VERSION_1_4};

        bool isRunning;
        SDL_Window* window;
        uint32_t windowWidth;
        uint32_t windowHeight;

        VkInstance vulkanInstance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator vmaAllocator;
        VkSurfaceKHR surface;

        uint32_t graphicsQueueFamilyIndex;
        VkQueue graphicsQueue;

        uint32_t swapchainWidth;
        uint32_t swapchainHeight;
        

        bool Initialize();
        void Run();
        void Shutdown();

    private:

        bool InitializeVulkan();

        bool CreateVulkanInstance();
        bool CreateSurface();
        bool GetPhysicalDevice();
        bool GetGraphicsQueue();
        bool CreateDevice();
        bool InitializeVMA();
        bool CreateSwapchain();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

};