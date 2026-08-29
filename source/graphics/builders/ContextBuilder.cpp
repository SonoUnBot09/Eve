#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include "ContextBuilder.hpp"
#include <graphics/GraphicsCore.hpp>
#include <EveSettings.hpp>
#include <eve/Debug.hpp>
#include <graphics/ErrorManager.hpp>

using namespace Debug;
using namespace Eve::Graphics;

bool ContextBuilder::Build(Context& context)
{
    if(!CreateInstance(context))
    {
        printError("Unable to create the vulkan instance");
        return false;
    }

    if(!ChoosePhysicalDevice(context))
    {
        printError("Unable to choose a GPU");
        return false;
    }

    if(!GetSurface(context))
    {
        printError("Unable to get the monitor surface");
        return false;
    }

    if(!GetGraphicsQueue(context))
    {
        printError("Unable to get a graphics queue");
        return false;
    }

    CreateDevice(context);

    if(!InitializeVMA(context))
    {
        printError("Unable to initialize VMA");
        return false;
    }

    return true;
}

bool ContextBuilder::CreateInstance(Context& context)
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

    std::vector<const char *> requestedExtensions;
    
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

    VkValidationFeatureEnableEXT enabledFeatures[] = { VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };

    VkValidationFeaturesEXT validationFeatures{};
    validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    validationFeatures.pNext = nullptr;
    validationFeatures.enabledValidationFeatureCount = 1;
    validationFeatures.pEnabledValidationFeatures = enabledFeatures;
    validationFeatures.disabledValidationFeatureCount = 0;
    validationFeatures.pDisabledValidationFeatures = nullptr;

    #pragma endregion

    VkInstanceCreateInfo instanceCI
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &validationFeatures,
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

    VK_CHECK(vkCreateInstance(&instanceCI, nullptr, &context.Instance));

    volkLoadInstance(context.Instance);

    return true;
}

bool ContextBuilder::ChoosePhysicalDevice(Context& context)
{
    uint32_t count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context.Instance, &count, nullptr));
    std::vector<VkPhysicalDevice> physicalDevices(count);
    VK_CHECK(vkEnumeratePhysicalDevices(context.Instance, &count, physicalDevices.data()));

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

    // Get GPU properties to use when the application is running
    vkGetPhysicalDeviceMemoryProperties(choseGPU, &context.PhysicalDeviceInfo.MemoryProperties);
    vkGetPhysicalDeviceProperties(choseGPU, &context.PhysicalDeviceInfo.Properties);
    if(context.PhysicalDeviceInfo.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        context.PhysicalDeviceInfo.isDedicated = true;
    }
    else 
    {
        context.PhysicalDeviceInfo.isDedicated = false;
    }

    context.PhysicalDevice = choseGPU;

    return true;
}

bool ContextBuilder::GetSurface(Context& context)
{
    if(!SDL_Vulkan_CreateSurface(GraphicsCore::Window.Window, context.Instance, nullptr, &context.Surface))
    {
        printError("Unable to create the surface");
        return false;
    }

    return true;
}

bool ContextBuilder::GetGraphicsQueue(Context& context)
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(context.PhysicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> queueFamProps(count, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    vkGetPhysicalDeviceQueueFamilyProperties2(context.PhysicalDevice, &count, queueFamProps.data());

    for (int i = 0; i < count; i++)
    {
        VkBool32 isPresentationSupported = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context.PhysicalDevice, i, context.Surface, &isPresentationSupported));

        if(queueFamProps[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && isPresentationSupported)
        {
            context.GraphicsQueueIndex = i;
            return true;
        }
    }

    printError("Unable to get a graphics queue with required properties");
    return false;
}

void ContextBuilder::CreateDevice(Context& context)
{
    #pragma region Features

    VkPhysicalDeviceMaintenance4FeaturesKHR maintenance4
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR,
        .pNext = nullptr
    };

    VkPhysicalDeviceDynamicRenderingFeaturesKHR availableDynamicRendering
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR, 
        .pNext = &maintenance4
    };
    VkPhysicalDeviceSynchronization2FeaturesKHR availableSync2
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR, 
        .pNext = &availableDynamicRendering
    };

    VkPhysicalDeviceVulkan12Features availableFeatures12
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, 
        .pNext = &availableSync2
    };
    VkPhysicalDeviceVulkan11Features availableFeatures11
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES, 
        .pNext = &availableFeatures12
    };
    VkPhysicalDeviceFeatures2 availableFeatures
    {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, 
        .pNext = &availableFeatures11
    };

    vkGetPhysicalDeviceFeatures2(context.PhysicalDevice, &availableFeatures);

    if(!maintenance4.maintenance4 || !availableDynamicRendering.dynamicRendering || !availableSync2.synchronization2 || 
       !availableFeatures12.timelineSemaphore || !availableFeatures.features.fillModeNonSolid || !availableFeatures12.bufferDeviceAddress || 
       !availableFeatures12.descriptorBindingPartiallyBound || !availableFeatures12.descriptorBindingVariableDescriptorCount || 
       !availableFeatures12.runtimeDescriptorArray || !availableFeatures12.shaderSampledImageArrayNonUniformIndexing ||
       !availableFeatures12.descriptorBindingSampledImageUpdateAfterBind || !availableFeatures12.descriptorBindingStorageImageUpdateAfterBind ||
       !availableFeatures12.descriptorBindingUniformBufferUpdateAfterBind || !availableFeatures12.descriptorBindingStorageBufferUpdateAfterBind ||
       !availableFeatures11.shaderDrawParameters || !availableFeatures.features.shaderInt64)
    {
        printError("Available device features do not respect application features requirement");
    }

    VkPhysicalDeviceMaintenance4FeaturesKHR maintenance4Features {};
    maintenance4Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES_KHR;
    maintenance4Features.pNext = nullptr;
    maintenance4Features.maintenance4 = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeatures{};
    dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeatures.pNext = &maintenance4Features;
    dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
    
    VkPhysicalDeviceSynchronization2FeaturesKHR sync2Features{};
    sync2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR;
    sync2Features.pNext = &dynamicRenderingFeatures;
    sync2Features.synchronization2 = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.pNext = &sync2Features;
    features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    features12.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingStorageImageUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
    features12.descriptorBindingPartiallyBound = VK_TRUE;
    features12.descriptorBindingVariableDescriptorCount = VK_TRUE;
    features12.runtimeDescriptorArray = VK_TRUE;
    features12.timelineSemaphore = VK_TRUE;
    features12.bufferDeviceAddress = VK_TRUE;

    VkPhysicalDeviceVulkan11Features features11{};
    features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features11.pNext = &features12;
    features11.shaderDrawParameters = VK_TRUE;

    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features.pNext = &features11;
    features.features.fillModeNonSolid = VK_TRUE;
    features.features.shaderInt64 = VK_TRUE;

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
    const std::vector<const char*> deviceExtensions { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
        VK_KHR_MAINTENANCE_4_EXTENSION_NAME
    };
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

    VK_CHECK(vkCreateDevice(context.PhysicalDevice, &deviceCI, nullptr, &context.Device));

    volkLoadDevice(context.Device);

    vkGetDeviceQueue(context.Device, context.GraphicsQueueIndex, 0, &context.GraphicsQueue);
}

bool ContextBuilder::InitializeVMA(Context& context)
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