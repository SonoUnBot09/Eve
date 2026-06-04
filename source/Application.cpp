#define VOLK_IMPLEMENTATION
#include "Application.hpp"

bool Application::Initialize()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "ERROR: Could not initiliaze SDL" << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Eve", 
        windowWidth, 
        windowHeight, 
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if(!window)
    {
        std::cerr << "ERROR: Could not create a window" << std::endl;
        return false;
    }



    return true;
}

bool Application::InitializeVulkan()
{
    if(!CreateVulkanInstance())
    {
        std::cerr << "ERROR: Could not create a vulkan instance" << std::endl;
        return false;
    }

    if(!CreateSurface())
    {
        std::cerr << "ERROR: Could not create a surface" << std::endl;
        return false;
    }

    if(!GetPhysicalDevice())
    {
        std::cerr << "ERROR: Could not find a physical device" << std::endl;
        return false;
    }

    if(!GetGraphicsQueue())
    {
        std::cerr << "ERROR: Could not find a graphics queue" << std::endl;
        return false;
    }

    if(!InitializeVMA())
    {
        std::cerr << "ERROR: Could not initialize VMA" << std::endl;
        return false;
    }

    return true;
}

bool Application::CreateVulkanInstance()
{
    if(volkInitialize() != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not initialize Volk" << std::endl;
    }

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "My first triangle",
        .apiVersion = vulkanVersion
    };

    
    uint32_t instanceExtensionCount = 0;
    const char *const *extensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);

    std::vector<const char *> requestedExtensions
    {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    for (int i = 0; i < instanceExtensionCount; i++)
    {
        requestedExtensions.push_back(extensions[i]);
    }

    std::vector<const char*> requestedLayers
    {
        "VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerCreateInfoEXT debugInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback
    };

    VkInstanceCreateInfo instanceCI
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
        .ppEnabledLayerNames = requestedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
        .ppEnabledExtensionNames = requestedExtensions.data()
    };

    if(vkCreateInstance(&instanceCI, nullptr, &vulkanInstance) != VK_SUCCESS)
    {
        return false;
    }

    volkLoadInstance(vulkanInstance);
    return true;
}
bool Application::CreateSurface()
{
    if(!SDL_Vulkan_CreateSurface(window, vulkanInstance, nullptr, &surface))
    {
        return false;
    }
    return true;
}
bool Application::GetPhysicalDevice()
{
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(vulkanInstance, &physicalDevicesCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
    vkEnumeratePhysicalDevices(vulkanInstance, &physicalDevicesCount, physicalDevices.data());

    if(physicalDevicesCount == 0)
    {
        return false;
    }

    VkPhysicalDevice chosePhysicalDevice;

    // Fallback if there is no dedicated gpu available
    chosePhysicalDevice = physicalDevices[0];

    for (VkPhysicalDevice pDev : physicalDevices)
    {
        VkPhysicalDeviceProperties pDevProperties;
        vkGetPhysicalDeviceProperties(pDev, &pDevProperties);

        if(pDevProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            chosePhysicalDevice = pDev;
            break;
        }
    }
    
    physicalDevice = chosePhysicalDevice;
    return true;
}
bool Application::GetGraphicsQueue()
{
    uint32_t familyQueueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &familyQueueCount, nullptr);
    std::vector<VkQueueFamilyProperties2> queueFamilyProperties (familyQueueCount, {
        .sType =  VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &familyQueueCount, queueFamilyProperties.data());

    for(int familyQueueIndex = 0; familyQueueIndex < familyQueueCount; familyQueueIndex++)
    {
        VkBool32 supportPresentation = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyQueueIndex, surface, &supportPresentation);

        VkQueueFamilyProperties2 properties = queueFamilyProperties[familyQueueIndex];
        if(properties.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && supportPresentation)
        {
            graphicsQueueFamilyIndex = familyQueueIndex;
            return true;
        }
    }

    return false;
}
bool Application::CreateDevice()
{
    VkPhysicalDeviceVulkan14Features requiredFeatures14 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr };
    VkPhysicalDeviceVulkan13Features requiredFeatures13 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &requiredFeatures14};
    VkPhysicalDeviceVulkan12Features requiredFeatures12 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &requiredFeatures13 };
    VkPhysicalDeviceFeatures2 requiredFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &requiredFeatures12};
    
    // Get available features
    vkGetPhysicalDeviceFeatures2(physicalDevice, &requiredFeatures);

    if(!requiredFeatures13.dynamicRendering || !requiredFeatures13.synchronization2 || !requiredFeatures12.timelineSemaphore)
    {
        std::cerr << "ERROR: Required physical device features not available" << std::endl;
        return false;
    }

    // If all requested features are available create a new set of features with the reals required features
    
    VkPhysicalDeviceVulkan14Features features14
    { 
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
        .pNext = nullptr,
    };
    VkPhysicalDeviceVulkan13Features features13 
    { 
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &features14,
        .synchronization2 = VK_TRUE,
        .dynamicRendering = VK_TRUE
    };
    VkPhysicalDeviceVulkan12Features features12 
    { 
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, 
        .pNext = &features13, 
        .timelineSemaphore = VK_TRUE
    };
    VkPhysicalDeviceFeatures2 features 
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features12
    };

    std::vector<float> queuePriorities{1.0f};
    VkDeviceQueueCreateInfo queueCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = queuePriorities.data(),
    };

    //Extension needed to present images on the screen
    const std::vector<const char *> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo deviceCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCI,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data()
    };

    if(vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not create the vulkan device" << std::endl;
        return false;
    }

    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    if(!graphicsQueue)
    {
        std::cerr << "ERROR: Could not get a graphics queue" << std::endl;
        return false;
    }

    return true;
}
bool Application::InitializeVMA()
{
    VmaVulkanFunctions vmaFunctionsInfo{};
    VmaAllocatorCreateInfo vmaCI
    {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = physicalDevice,
        .device = device,
        .pVulkanFunctions = &vmaFunctionsInfo,
        .instance = vulkanInstance,
        .vulkanApiVersion = vulkanVersion
    };

    if(vmaImportVulkanFunctionsFromVolk(&vmaCI, &vmaFunctionsInfo) != VK_SUCCESS)
    {
        return false;
    }

    if(vmaCreateAllocator(&vmaCI, &vmaAllocator) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

bool Application::CreateSwapchain()
{
    swapchainWidth = windowWidth;
    swapchainHeight = windowHeight;

    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCaps) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not get surface capabilities" << std::endl;
        return false;
    }

    

    return true;
}

void Application::Shutdown()
{
    if(vmaAllocator)
    {
        vmaDestroyAllocator(vmaAllocator);
    }

    if(surface)
    {
        vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);
    }

    if(device)
    {
        vkDestroyDevice(device, nullptr);
    }

    if(vulkanInstance)
    {
        vkDestroyInstance(vulkanInstance, nullptr);
    }
    volkFinalize();

    if(window)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}