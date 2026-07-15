#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include <Eve/graphics/Buffer.hpp>
#include <Eve/graphics/Texture.hpp>
#include <graphics/api/vk/Context.hpp>
#include <graphics/api/vk/Swapchain.hpp>
#include <graphics/api/vk/Window.hpp>

#include <iostream>

namespace Eve::Graphics
{
    class ContextBuilder
    {
        public:

            static Context& Build(Window _window, bool& success);
            
            inline static Context context;
        private:

            inline static bool isInitialized = false;

            inline static Window window;

            static bool CreateInstance();
            static bool ChoosePhysicalDevice();
            static bool GetSurface();
            static bool GetGraphicsQueue();
            static bool CreateDevice();
            static bool InitializeVMA();

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
}