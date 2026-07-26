#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "Context.hpp"

#include <iostream>

namespace Eve::Graphics
{
    class ContextBuilder
    {
        public:

            static bool Build(Context& context);
            
        private:

            static bool CreateInstance(Context& context);
            static bool ChoosePhysicalDevice(Context& context);
            static bool GetSurface(Context& context);
            static bool GetGraphicsQueue(Context& context);
            static bool CreateDevice(Context& context);
            static bool InitializeVMA(Context& context);

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