#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include <graphics/api/vk/VkContextBuilder.hpp>
#include <Eve/Debug.hpp>

using namespace Debug;

bool VkContextBuilder::Build(Window& _window, Context& _context, Swapchain& _swapchain)
{
    if(isInitialized)
    {
        printError("Vulkan context already initialized, unable to initialize it again");
        return false;
    }

    if(!CreateInstance())
    {
        printError("Unable to create a graphic instance");
        return false;
    }

    if(!ChoosePhysicalDevice())
    {
        printError("Unable to choose a GPU");
        return false;
    }

    if(!GetSurface())
    {
        printError("Unable to get the monitor surface");
        return false;
    }

    if(!GetGraphicsQueue())
    {
        printError("Unable to get a graphics queue");
        return false;
    }

    if(!CreateDevice())
    {
        printError("Unable to create a logical device");
        return false;
    }

    if(!InitializeVMA())
    {
        printError("Unable to initialize VMA");
        return false;
    }

    if(!CreateSwapchain())
    {
        printError("Unable to create the swapchain");
        return false;
    }

    _window = window;
    _context = context;
    _swapchain = swapchain; 

    isInitialized = true;

    return true;
}

bool VkContextBuilder::CreateWindow()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printError("Unable to initialize SDL");
        return false;
    }

    window.Window = SDL_CreateWindow("Eve", window.Width, window.Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

    if(!window.Window)
    {
        printError("Unable to create the window");
        return false;
    }
}

bool VkContextBuilder::CreateInstance()
{
    // initialize volk
    if(volkInitialize() != VK_SUCCESS)
    {
        printError("Unable to initialize Volk");
        return false;
    }


    VkApplicationInfo appInfo
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Eve",
        .apiVersion = vulkanVersion
    };

    #pragma region Extensions and layers
    uint32_t extensionsCount = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionsCount);

    std::vector<const char *> requestedExtensions
    {
        //VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    for (int i = 0; i < extensionsCount; i++)
    {
        requestedExtensions.push_back(extensions[i]);
    }

    std::vector<const char*> requestedLayers
    {
        //"VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerCreateInfoEXT debugInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = PrintVulkanMessages
    };
    #pragma endregion

    VkInstanceCreateInfo instanceCI
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
        .ppEnabledLayerNames = requestedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
        .ppEnabledExtensionNames = requestedExtensions.data()
    };

    if(vkCreateInstance(&instanceCI, nullptr, &context.Instance) != VK_SUCCESS)
    {
        printError("Unable to create the vulkan instance");
        return false;
    }
    
    volkLoadInstance(context.Instance);

    return true;
}

bool VkContextBuilder::ChoosePhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(context.Instance, &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkEnumeratePhysicalDevices(context.Instance, &count, physicalDevices.data());

    if(count == 0)
    {
        printError("No physical device detected");
        return false;
    }

    VkPhysicalDevice choseGPU = physicalDevices[0];

    for (int i = 0; i < count; i++)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

        if(properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            choseGPU = physicalDevices[i];
            break;
        }
    }

    context.PhysicalDevice = choseGPU;

    return true;
}

bool VkContextBuilder::GetSurface()
{
    if(!SDL_Vulkan_CreateSurface(window.Window, context.Instance, nullptr, &context.Surface))
    {
        printError("Unable to create the surface");
        return false;
    }

    return true;
}

bool VkContextBuilder::GetGraphicsQueue()
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(context.PhysicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> queueFamProps(count, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    vkGetPhysicalDeviceQueueFamilyProperties2(context.PhysicalDevice, &count, queueFamProps.data());

    for (int i = 0; i < count; i++)
    {
        VkBool32 isPresentationSupported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(context.PhysicalDevice, i, context.Surface, &isPresentationSupported);

        if(queueFamProps[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && isPresentationSupported)
        {
            context.GraphicsQueueIndex = i;
            return true;
        }
    }

    printError("Unable to get a graphics queue with required properties");

    return false;
}

bool VkContextBuilder::CreateDevice()
{
    #pragma region Features

    VkPhysicalDeviceVulkan14Features availableFeatures14 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
    VkPhysicalDeviceVulkan13Features availableFeatures13 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &availableFeatures14};
    VkPhysicalDeviceVulkan12Features availableFeatures12 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &availableFeatures13};
    VkPhysicalDeviceFeatures2 availableFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &availableFeatures12};

    vkGetPhysicalDeviceFeatures2(context.PhysicalDevice, &availableFeatures);

    // Check if all the required features are available on the machine 
    if(!availableFeatures13.dynamicRendering || !availableFeatures13.synchronization2 || !availableFeatures12.timelineSemaphore || 
       !availableFeatures.features.fillModeNonSolid || !availableFeatures12.bufferDeviceAddress)
    {
        printError("Available device features do not respect application features requirement");
    }

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
        .timelineSemaphore = VK_TRUE,
        .bufferDeviceAddress = VK_TRUE
    };

    VkPhysicalDeviceFeatures2 features
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features12,
        .features
        {
            .fillModeNonSolid = VK_TRUE
        }
    };

    #pragma endregion

    #pragma region Queues

    std::vector<float> graphicsPriority {1.0f};
    VkDeviceQueueCreateInfo graphicsQueueCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = context.GraphicsQueueIndex,
        .queueCount = 1,
        .pQueuePriorities = graphicsPriority.data()
    };

    #pragma endregion

    #pragma region Extensions
    const std::vector<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    #pragma endregion

    VkDeviceCreateInfo deviceCI
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &graphicsQueueCI,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data()
    };

    if(vkCreateDevice(context.PhysicalDevice, &deviceCI, nullptr, &context.Device) != VK_SUCCESS)
    {
        printError("Unable to create the vulkan device");
        return false;
    }

    vkGetDeviceQueue(context.Device, context.GraphicsQueueIndex, 0, &context.GraphicsQueue);
    if(!context.GraphicsQueue)
    {
        printError("Unable to get the graphics queue");
        return false;
    }

    return true;
}

bool VkContextBuilder::InitializeVMA()
{
    VmaVulkanFunctions vmaFunctionsInfo {};
    VmaAllocatorCreateInfo allocatorCI
    {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = context.PhysicalDevice,
        .device = context.Device,
        .pVulkanFunctions = &vmaFunctionsInfo,
        .instance = context.Instance,
        .vulkanApiVersion = vulkanVersion
    };

    if(vmaImportVulkanFunctionsFromVolk(&allocatorCI, &vmaFunctionsInfo) != VK_SUCCESS)
    {
        printError("Unable to load Vulkan functions in VMA with Volk");
        return false;
    }

    if(vmaCreateAllocator(&allocatorCI, &context.Allocator) != VK_SUCCESS)
    {
        printError("Unable to create the VMA allocator");
        return false;
    }

    return true;
}

bool VkContextBuilder::CreateSwapchain()
{
    swapchain.Width = window.Width;
    swapchain.Height = window.Height;

    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.PhysicalDevice, context.Surface, &surfaceCaps) != VK_SUCCESS)
    {
        printError("Unable to get surface capabilities");
        return false;
    }

    uint32_t swapchainImagesCount = std::max(2u, surfaceCaps.minImageCount);
    if(surfaceCaps.maxImageCount == 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.PhysicalDevice, context.Surface, &supportedFormatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.PhysicalDevice, context.Surface, &supportedFormatsCount, supportedFormats.data());

    bool isRequiredFormatSupported = false;
    VkFormat chooseFormat = VK_FORMAT_UNDEFINED;

    // Fallback for any format supported
    if(supportedFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        isRequiredFormatSupported = true;
        chooseFormat = swapchainFormats[0];
    }

    for (const VkFormat proposedFormat : swapchainFormats)
    {
        for (const VkSurfaceFormatKHR validFormat : supportedFormats)
        {
            if(proposedFormat == validFormat.format)
            {
                isRequiredFormatSupported = true;
                chooseFormat = proposedFormat;
                break;
            }
        }
    }

    if(!isRequiredFormatSupported)
    {
        printError("The device does not support the required swapchain format");
    }

    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = context.Surface,
        .minImageCount = swapchainImagesCount,
        .imageFormat = chooseFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent {.width = swapchain.Width, .height = swapchain.Height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR
    };

    if(vkCreateSwapchainKHR(context.Device, &swapchainCI, nullptr, &swapchain.Swapchain) != VK_SUCCESS)
    {
        printError("Unable to create the swapchain");
        return false;
    }

    // Swapchain images
    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(context.Device, swapchain.Swapchain, &imagesCount, nullptr);
    swapchain.SwapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(context.Device, swapchain.Swapchain, &imagesCount, swapchain.SwapchainImages.data());

    swapchain.SwapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain.SwapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = chooseFormat,
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if(vkCreateImageView(context.Device, &imageViewCI, nullptr, &swapchain.SwapchainImageViews[i]) != VK_SUCCESS)
        {
            printError("Unable to create swapchain image views");
            return false;
        }
    }

    return true;
}

void VkContextBuilder::DestroySwapchain()
{
    for(uint32_t i = 0; i < swapchain.SwapchainImageViews.size(); i++)
    {
        vkDestroyImageView(context.Device, swapchain.SwapchainImageViews[i], nullptr);
    }
    swapchain.SwapchainImageViews.clear();

    if(swapchain.Swapchain)
    {
        vkDestroySwapchainKHR(context.Device, swapchain.Swapchain, nullptr);
    }
    swapchain.SwapchainImages.clear();
    swapchain.Swapchain = nullptr;
}

VkContextBuilder::~VkContextBuilder()
{
    DestroySwapchain();

    if(context.Allocator)
    {
        vmaDestroyAllocator(context.Allocator);
    }

    if(context.Surface)
    {
        vkDestroySurfaceKHR(context.Instance, context.Surface, nullptr);
    }

    if(context.Device)
    {
        vkDestroyDevice(context.Device, nullptr);
    }

    if(context.Instance)
    {
        vkDestroyInstance(context.Instance, nullptr);
    }

    volkFinalize();

    if(window.Window)
    {
        SDL_DestroyWindow(window.Window);
    }

    SDL_Quit();
}