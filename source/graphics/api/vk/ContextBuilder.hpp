#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include <Eve/graphics/Types.hpp>
#include <graphics/api/vk/Context.hpp>
#include <graphics/api/vk/Swapchain.hpp>
#include <graphics/api/vk/Window.hpp>

#include <iostream>

using namespace Eve::Graphics;

class ContextBuilder
{
    public:

        bool useValidationLayers = false;
        Context& Build(Window _window, bool& success);
        
    private:

        inline static bool isInitialized = false;
        static constexpr uint32_t vulkanVersion {VK_API_VERSION_1_4};
        static constexpr VkFormat swapchainFormats[] 
        {
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_FORMAT_B8G8R8A8_SRGB
        };

        Window window;
        Context context;

        bool CreateInstance();
        bool ChoosePhysicalDevice();
        bool GetSurface();
        bool GetGraphicsQueue();
        bool CreateDevice();
        bool InitializeVMA();

        // Vulkan dedicated func to print out errors
        static inline VKAPI_ATTR VkBool32 VKAPI_CALL PrintVulkanMessages( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                            void *pUserData)
        {
            if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            {
               std::cerr << pCallbackData->pMessage << std::endl;
            }
                                                                
            return VK_FALSE;
        }
};