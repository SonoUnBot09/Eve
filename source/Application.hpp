#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <Volk/volk.h>
#include <shaderc/shaderc.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include <shaderc/shaderc.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <vector>
#include <array>
#include <string>

#include <components/Transform.hpp>
#include <components/Camera.hpp>
#include <Eve/Entities/EntityManager.hpp>
#include <Eve/Entities/SystemDispatcher.hpp>
#include <Eve/Entities/EntityCommandInfo.hpp>
#include <Eve/Entities/EntityManager.hpp>
#include <Eve/Entities/QueryInfo.hpp>

#include <Eve/Debug.hpp>
#include <Eve/Utils.hpp>

using namespace Debug;
using namespace Utils;

class Application
{
    public:

        bool Initialize();
        void Start();
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

        struct Buffer
        {
            VkBuffer bufferHandle;
            VmaAllocation allocation;
            VmaAllocationInfo allocationInfo;
        };

        struct Texture
        {
            VkImage image;
            VkImageView imageView;
            VkSampler sampler;
            VmaAllocation allocation;
            VmaAllocationInfo allocationInfo;
        };

        struct Vertex 
        {
            float position[3];
            float color[3];
            float uv[2];
        };

        struct PushConstant
        {
            glm::mat4 mvp;
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

        std::vector<Vertex> vertices
        {
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            // Retro (-Z)
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            // Sinistra (-X)
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            // Destra (+X)
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            // Sopra (+Y)
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

            // Sotto (-Y)
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
        };

        std::vector<uint16_t> indices = {
            0, 1, 2,    0, 2, 3,    // Fronte
            4, 5, 6,    4, 6, 7,    // Retro
            8, 9, 10,   8, 10, 11,  // Sinistra
            12, 13, 14, 12, 14, 15, // Destra
            16, 17, 18, 16, 18, 19, // Sopra
            20, 21, 22, 20, 22, 23, // Sotto
        };

        Buffer vertexBuffer;
        Buffer indexBuffer;
        Texture texture;

        VkDescriptorSetLayout descriptorLayout;
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;

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

        void CreateMeshBuffers();
        void CreateDescriptor();
        void CreateTexture();
        void LoadTextureData(unsigned char *source, int width, int height);

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