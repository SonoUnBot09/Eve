#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include "Application.hpp"

bool Application::Initialize()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printError("Could not initialize SDL");
        return false;
    }

    window = SDL_CreateWindow("Eve", 
        windowWidth, 
        windowHeight, 
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if(!window)
    {
        printError("Could not create a window");
        return false;
    }

    if(!InitializeVulkan())
    {
        printError("Could not initialize Vulkan");
        return false;
    }
    
    return true;
}

bool Application::InitializeVulkan()
{
    if(!CreateVulkanInstance())
    {
        printError("Could not create a vulkan instance");
        return false;
    }

    if(!CreateSurface())
    {
        printError("Could not create a surface");
        return false;
    }

    if(!GetPhysicalDevice())
    {
        printError("Could not find a physical device");
        return false;
    }

    if(!GetGraphicsQueue())
    {
        printError("Could not find a graphics queue");
        return false;
    }

    if(!CreateDevice())
    {
        printError("Could not create the logical device");
        return false;
    }

    if(!InitializeVMA())
    {
        printError("Could not initialize VMA");
        return false;
    }

    if(!CreateSwapchain())
    {
        printError("Could not create the swapchain");
        return false;
    }

    if(!CreateShaders())
    {
        printError("Could not create the shaders");
        return false;
    }

    if(!CreateGraphicsPipeline())
    {
        printError("Could not create the graphics pipeline");
        return false;
    }

    return true;
}

bool Application::CreateVulkanInstance()
{
    if(volkInitialize() != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not initialize Volk" << std::endl;
        return false;
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

    uint32_t requestedImagesCount = std::max(2u, surfaceCaps.minImageCount);
    if(surfaceCaps.maxImageCount != 0)
    {
        // we have limited images and not infinite
        requestedImagesCount = std::min(requestedImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t formatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats (formatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatsCount, supportedSurfaceFormats.data());

    bool isRequiredFormatSupported = false;
    for(const VkSurfaceFormatKHR format : supportedSurfaceFormats)
    {
        if(format.format == swapchainFormat)
        {
            isRequiredFormatSupported = true;
            break;
        }
    }

    if(!isRequiredFormatSupported)
    {
        std::cerr << "ERROR: The required surface format is not supported" << std::endl;
        return false;
    }

    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = requestedImagesCount,
        .imageFormat = swapchainFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent {.width = swapchainWidth, .height = swapchainHeight},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    if(vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapchain) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not create the swapchain" << std::endl;
        return false;
    }

    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, nullptr);
    swapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, swapchainImages.data());

    // Create swapchain image views for each swapchain image
    swapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchainFormat,
            .subresourceRange 
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if(vkCreateImageView(device, &imageViewCI, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            std::cerr << "ERROR: Could not create an image view" << std::endl;
            return false;
        }
    }

    // Create the semaphores used to signal render completition
    renderCompleteSemaphores.resize(imagesCount);
    for (int i = 0; i < imagesCount; i++)
    {
        VkSemaphoreCreateInfo semaphoreCI {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        if(vkCreateSemaphore(device, &semaphoreCI, nullptr, &renderCompleteSemaphores[i]) != VK_SUCCESS)
        {
            std::cerr << "ERROR: Could not create render completition semaphores" << std::endl;
            return false;
        }
    }

    // Depth image creation
    VkImageCreateInfo depthImageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depthFormat,
        .extent {.width = swapchainWidth, .height = swapchainHeight, .depth = 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocationCI
    {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    if(vmaCreateImage(vmaAllocator, &depthImageCI, &allocationCI, 
        &depthImage, &depthImageAllocation, nullptr) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not create depth image" << std::endl;
        return false;
    }

    VkImageViewCreateInfo depthImageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = depthImage,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = depthFormat,
        .subresourceRange {.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, .baseMipLevel = 0, .levelCount = 1 , .baseArrayLayer = 0, .layerCount = 1}
    };

    if(vkCreateImageView(device, &depthImageViewCI, nullptr, &depthImageView) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Could not create the depth image view" << std::endl;
        return false;
    }

    return true;
}

bool Application::CreateShaders()
{
    vertexShader = CreateShaderModule("shader.vert", shaderc_vertex_shader);
    if(!vertexShader)
    {
        return false;
    }

    fragmentShader = CreateShaderModule("shader.frag", shaderc_fragment_shader);
    if(!fragmentShader)
    {
        return false;
    }

    return true;
}

bool Application::CreateGraphicsPipeline()
{
    VkPipelineLayoutCreateInfo pipelineLayoutCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0
    };

    if(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &pipelineLayout) != VK_SUCCESS)
    {
        printError("Could not create the graphics pipeline layout");
        return false;
    }

    const char *entryFuncName = "main";
    std::vector<VkPipelineShaderStageCreateInfo> shadersStage
    {
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader,
            .pName = entryFuncName
        },
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader,
            .pName = entryFuncName
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .stencilTestEnable = VK_FALSE
    };

    VkPipelineViewportStateCreateInfo viewportInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multiSampleInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState attachmentState
    {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendStateCreateInfo blendInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &attachmentState
    };

    std::vector<VkDynamicState> dynamicState
    {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicState.size()),
        .pDynamicStates = dynamicState.data()
    };

    VkPipelineRenderingCreateInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &swapchainFormat,
        .depthAttachmentFormat = depthFormat
    };

    VkGraphicsPipelineCreateInfo pipelineCI
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = static_cast<uint32_t>(shadersStage.size()),
        .pStages = shadersStage.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterInfo,
        .pMultisampleState = &multiSampleInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &blendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = pipelineLayout,
        .renderPass = VK_NULL_HANDLE
    };

    if(vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCI, nullptr, &pipeline) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

VkShaderModule Application::CreateShaderModule(const std::string shaderName, const shaderc_shader_kind shaderKind)
{
    const std::string shaderPath = "../source/shaders/" + shaderName;
    std::string src = readTextFile(shaderPath);
    if(src.empty())
    {
        printError("Shader '" + shaderName +  "' at path '" + shaderPath + "' does not exists");
        return nullptr;
    }

    shaderc::Compiler compiler;
    shaderc::CompileOptions compilerOptions;

    compilerOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    compilerOptions.SetTargetSpirv(shaderc_spirv_version_1_6);
    compilerOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(src, shaderKind, shaderName.c_str(), compilerOptions);

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        printError("Compilation Error | " + result.GetErrorMessage());
        return nullptr;
    }

    const size_t shaderSize = (result.cend() - result.cbegin()) * sizeof(uint32_t);

    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderSize,
        .pCode = result.cbegin()
    };

    VkShaderModule shaderModule = nullptr;
    if(vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Could not create the shader module");
        return nullptr;
    }

    return shaderModule;
}

void Application::DestroySwapchain()
{
    for (VkImageView &swapchainImageView : swapchainImageViews)
    {
        vkDestroyImageView(device, swapchainImageView, nullptr);
    }
    swapchainImageViews.clear();

    for(VkSemaphore &semaphore : renderCompleteSemaphores)
    {
        vkDestroySemaphore(device, semaphore, nullptr);
    }
    renderCompleteSemaphores.clear();

    if(swapchain)
    {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = nullptr;
    }

    if(depthImageView)
    {
        vkDestroyImageView(device, depthImageView, nullptr);
        vmaDestroyImage(vmaAllocator, depthImage, depthImageAllocation);

        depthImageView = nullptr;
        depthImage = nullptr;
        depthImageAllocation = nullptr;
    }
}

void Application::Shutdown()
{

    if(pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    if(pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, pipeline, nullptr);
    }

    if(vertexShader)
    {
        vkDestroyShaderModule(device, vertexShader, nullptr);
    }

    if(fragmentShader)
    {
        vkDestroyShaderModule(device, fragmentShader, nullptr);
    }

    DestroySwapchain();

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