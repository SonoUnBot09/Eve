#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include <shaderc/shaderc.hpp>

#include <vector>


#include "Debug.hpp"
#include "Utils.hpp"

using namespace Debug;
using namespace Utils;

class Application
{
    public:

        bool Initialize();
        void Run();
        void Shutdown();

    private:

        struct FrameData
        {
            VkSemaphore acquiredImageSemaphore;
            VkSemaphore renderCompletedSemaphore;

            VkCommandPool commandPool;
            VkCommandBuffer commandBuffer;

            VkImage depthImage;
            VkImageView depthImageView;
            VmaAllocation depthImageAllocation;
        };

        static constexpr uint32_t vulkanVersion {VK_API_VERSION_1_4};
        static constexpr VkFormat swapchainFormat {VK_FORMAT_R8G8B8A8_SRGB};
        static constexpr VkFormat depthFormat {VK_FORMAT_D32_SFLOAT};
        static constexpr uint32_t maxFramesInFlight = 2;

        uint64_t frameIndex = 2;

        uint32_t windowWidth = 512;
        uint32_t windowHeight = 512;

        uint32_t swapchainWidth = 0;
        uint32_t swapchainHeight = 0;
        
        bool isSwapchainRecreationNeeded = false;
        bool isAppRunning = true;

        SDL_Window* window = nullptr;

        VkInstance instance = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice device = nullptr;

        VkSurfaceKHR surface = nullptr;

        VkSwapchainKHR swapchain = nullptr;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;

        VkQueue graphicsQueue = nullptr;
        uint32_t graphicsQueueFamIndex;

        VmaAllocator vmaAllocator = nullptr;

        std::vector<FrameData> frameData;

        VkPipelineLayout grapchisPipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;

        VkShaderModule vertexShader = nullptr;
        VkShaderModule fragmentShader = nullptr;

        VkSemaphore timelineSemaphore = nullptr;

        bool InitializeVulkan();
        void Render();

        bool CreateVulkanInstance();
        bool GetPhysicalDevice();
        bool CreateSurface();
        bool GetGraphicsQueue();
        bool CreateDevice();
        bool InitializeVMA();
        bool CreateSwapchain();
        bool CreateShaders();
        bool CreateGraphicsPipeline();
        bool CreateSyncResource();
        bool CreateCommandBuffers();

        VkShaderModule CreateShaderModule(std::string fileName, shaderc_shader_kind kind);
        void DestroySwapchain();


        // vulkan dedicated func to print out errors
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