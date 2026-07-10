#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include <graphics/api/vk/ContextBuilder.hpp>
#include <EveSettings.hpp>
#include <Eve/Debug.hpp>

using namespace Debug;
using namespace Eve::Graphics;

Context& ContextBuilder::Build(Window _window, bool& success)
{

    if(isInitialized)
    {
        printError("Vulkan context already initialized, unable to initialize it again");
        success = false;
        return context;
    }

    window = _window;

    if(!CreateInstance())
    {
        printError("Unable to create a graphic instance");
        success = false;
        return context;
    }

    if(!ChoosePhysicalDevice())
    {
        printError("Unable to choose a GPU");
        success = false;
        return context;
    }

    if(!GetSurface())
    {
        printError("Unable to get the monitor surface");
        success = false;
        return context;
    }

    if(!GetGraphicsQueue())
    {
        printError("Unable to get a graphics queue");
        success = false;
        return context;
    }

    if(!CreateDevice())
    {
        printError("Unable to create a logical device");
        success = false;
        return context;
    }

    if(!InitializeVMA())
    {
        printError("Unable to initialize VMA");
        success = false;
        return context;
    }


    isInitialized = true;
    success = true;

    return context;
}

bool ContextBuilder::CreateInstance()
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
        .apiVersion = Eve::Settings::vulkanVersion
    };

    #pragma region Extensions and layers
    uint32_t extensionsCount = 0;
    const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&extensionsCount);

    std::vector<const char *> requestedExtensions
    {
        "VK_EXT_descriptor_indexing"
    };
    if(Eve::Settings::useValidationLayers)
    {
        requestedExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    for (int i = 0; i < extensionsCount; i++)
    {
        requestedExtensions.push_back(extensions[i]);
    }

    std::vector<const char*> requestedLayers;
    if(Eve::Settings::useValidationLayers)
    {
        requestedLayers.push_back("VK_LAYER_KHRONOS_validation");
    }

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

    if(Eve::Settings::useValidationLayers)
    {
        instanceCI.pNext = &debugInfo;
    }

    if(vkCreateInstance(&instanceCI, nullptr, &context.Instance) != VK_SUCCESS)
    {
        printError("Unable to create the vulkan instance");
        return false;
    }
    
    volkLoadInstance(context.Instance);

    return true;
}

bool ContextBuilder::ChoosePhysicalDevice()
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

bool ContextBuilder::GetSurface()
{
    if(!SDL_Vulkan_CreateSurface(window.Window, context.Instance, nullptr, &context.Surface))
    {
        printError("Unable to create the surface");
        return false;
    }

    return true;
}

bool ContextBuilder::GetGraphicsQueue()
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

bool ContextBuilder::CreateDevice()
{
    #pragma region Features

    VkPhysicalDeviceVulkan14Features availableFeatures14 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
    VkPhysicalDeviceVulkan13Features availableFeatures13 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &availableFeatures14};
    VkPhysicalDeviceVulkan12Features availableFeatures12 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &availableFeatures13};
    VkPhysicalDeviceFeatures2 availableFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &availableFeatures12};

    vkGetPhysicalDeviceFeatures2(context.PhysicalDevice, &availableFeatures);

    // Check if all the required features are available on the machine 
    if(!availableFeatures13.dynamicRendering || !availableFeatures13.synchronization2 || !availableFeatures12.timelineSemaphore || 
       !availableFeatures.features.fillModeNonSolid || !availableFeatures12.bufferDeviceAddress || !availableFeatures12.descriptorBindingPartiallyBound ||
       !availableFeatures12.descriptorBindingVariableDescriptorCount || !availableFeatures12.runtimeDescriptorArray || 
       !availableFeatures12.shaderSampledImageArrayNonUniformIndexing)
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
        .bufferDeviceAddress = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE
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

bool ContextBuilder::InitializeVMA()
{
    VmaVulkanFunctions vmaFunctionsInfo {};
    VmaAllocatorCreateInfo allocatorCI
    {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = context.PhysicalDevice,
        .device = context.Device,
        .pVulkanFunctions = &vmaFunctionsInfo,
        .instance = context.Instance,
        .vulkanApiVersion = Eve::Settings::vulkanVersion
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