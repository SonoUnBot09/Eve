#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <vulkan/vulkan.h>

#include <vma/vk_mem_alloc.h>

#include <shaderc/shaderc.hpp>

#include <vector>
#include <iostream>

#include "Debug.hpp"
#include "Utils.hpp"

using namespace Debug;
using namespace Utils;

class Application
{

    public:

        static constexpr uint32_t vulkanVersion {VK_API_VERSION_1_4};
        static constexpr VkFormat swapchainFormat {VK_FORMAT_R8G8B8A8_SRGB};
        static constexpr VkFormat depthFormat {VK_FORMAT_D32_SFLOAT};

        bool isRunning;
        SDL_Window* window;

        uint32_t windowWidth = 512;
        uint32_t windowHeight = 512;

        uint32_t swapchainWidth;
        uint32_t swapchainHeight;

        VkInstance vulkanInstance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator vmaAllocator;
        VkSurfaceKHR surface;

        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkSemaphore> renderCompleteSemaphores;

        VkImage depthImage;
        VkImageView depthImageView;
        VmaAllocation depthImageAllocation;

        uint32_t graphicsQueueFamilyIndex;
        VkQueue graphicsQueue;
        
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

        VkShaderModule vertexShader;
        VkShaderModule fragmentShader;

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
        bool CreateShaders();
        bool CreateGraphicsPipeline();

        VkShaderModule CreateShaderModule(std::string shaderName, shaderc_shader_kind shaderKind);

        void DestroySwapchain();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

};