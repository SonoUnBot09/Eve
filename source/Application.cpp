#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Application.hpp"

bool Application::Initialize()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printError("Unable to initialize SDL");
        return false;
    }

    window = SDL_CreateWindow("Eve", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

    if(!window)
    {
        printError("Unable to create the window");
        return false;
    }

    if(!InitializeVulkan())
    {
        printError("Unable to initialize Vulkan");
        return false;
    }

    return true;
}

void Application::Run()
{
    while(isAppRunning)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
            {
                isAppRunning = false;
                break;
            }
            else if(event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
            }
        }

        Render();
        
    }
}

void Application::Render()
{

    if(isSwapchainRecreationNeeded)
    {
        vkDeviceWaitIdle(device);
        DestroySwapchain();
        CreateSwapchain();
        isSwapchainRecreationNeeded = false;
    }

    uint32_t frameDataIndex = frameIndex++ % maxFramesInFlight;
    uint64_t signalValue = frameIndex + 1;
    uint64_t waitValue = signalValue - maxFramesInFlight;

    VkSemaphoreWaitInfo waitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &timelineSemaphore,
        .pValues = &waitValue
    };

    vkWaitSemaphores(device, &waitInfo, UINT64_MAX);

    FrameData &data = frameData[frameDataIndex]; 
    
    vkResetCommandPool(device, data.commandPool, 0);

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, data.acquiredImageSemaphore, nullptr, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        isSwapchainRecreationNeeded = true;
        return;
    }
    else if(result == VK_SUBOPTIMAL_KHR)
    {
        isSwapchainRecreationNeeded = true;
    }

    std::vector<VkImageMemoryBarrier2> layoutBarriers
    {
        VkImageMemoryBarrier2
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = swapchainImages[imageIndex],
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        },
        VkImageMemoryBarrier2
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
            .dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .image = data.depthImage,
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }
    };
    VkImageMemoryBarrier2 presentationImageBarrier
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = swapchainImages[imageIndex],
        .subresourceRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkCommandBufferBeginInfo beginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(data.commandBuffer, &beginInfo);

    VkDependencyInfo layoutDepInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32_t>(layoutBarriers.size()),
        .pImageMemoryBarriers = layoutBarriers.data()
    };

    vkCmdPipelineBarrier2(data.commandBuffer, &layoutDepInfo);

    VkRenderingAttachmentInfo colorAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = swapchainImageViews[imageIndex],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.color{.float32{0.0, 0.0,0.0, 1.0}}}
    };

    VkRenderingAttachmentInfo depthAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = data.depthImageView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue = {.depthStencil{.depth = 1, .stencil = 0}}
    };

    VkRenderingInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = swapchainWidth, .height = swapchainHeight}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo
    };

    vkCmdBeginRendering(data.commandBuffer, &renderInfo);
    {

        VkViewport viewport
        {
            .x = 0, .y = 0,
            .width = static_cast<float>(swapchainWidth),
            .height =  static_cast<float>(swapchainHeight),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = swapchainWidth, .height = swapchainHeight}
        };

        vkCmdSetViewport(data.commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(data.commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(data.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        const float fov = 60.0f;
        const float aspect = static_cast<float>(swapchainWidth) / static_cast<float>(swapchainHeight);
        const float nearPlane = 0.1f;
        const float farPlane = 100.0f;

        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0, 0, 6));
        //model = glm::translate(model, glm::vec3(std::sin(frameIndex * 0.1f), std::sin(frameIndex * 0.15), std::sin(frameIndex * 0.07)));
        model = glm::rotate(model, std::sin(frameIndex * 0.05f), glm::vec3(0, 1, 0));
        model = glm::rotate(model, std::sin(frameIndex * 0.02f), glm::vec3(0.5f, 0, 0.1f));
        glm::mat4 view = glm::lookAt(
            glm::vec3(0, 0, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 1, 0)
        );
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
        projection[1][1] *= -1;
        glm::mat4 mvp = projection * view * model;
        
        PushConstant pushConstantData
        {
            .mvp = mvp
        };

        vkCmdPushConstants(
            data.commandBuffer, 
            grapchisPipelineLayout, 
            VK_SHADER_STAGE_VERTEX_BIT, 
            0, 
            sizeof(PushConstant),
            &pushConstantData
        );
    
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(data.commandBuffer, 0, 1, &vertexBuffer.bufferHandle, &offset);
        vkCmdBindIndexBuffer(data.commandBuffer, indexBuffer.bufferHandle, offset, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(data.commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        vkCmdDraw(data.commandBuffer, 36, 1, 0, 0);

    }
    vkCmdEndRendering(data.commandBuffer);


    VkDependencyInfo presentDepInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &presentationImageBarrier
    };

    vkCmdPipelineBarrier2(data.commandBuffer, &presentDepInfo);

    vkEndCommandBuffer(data.commandBuffer);

    VkCommandBufferSubmitInfo cmdSubmitInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = data.commandBuffer
    };

    VkSemaphoreSubmitInfo waitInfoSemaphores
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = data.acquiredImageSemaphore,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    std::vector<VkSemaphoreSubmitInfo> signalInfoSemaphores
    {
        VkSemaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = data.renderCompletedSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
        },
        VkSemaphoreSubmitInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = timelineSemaphore,
            .value = signalValue,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
        }
    };

    VkSubmitInfo2 submitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .waitSemaphoreInfoCount = 1,
        .pWaitSemaphoreInfos = &waitInfoSemaphores,

        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdSubmitInfo,

        .signalSemaphoreInfoCount = static_cast<uint32_t>(signalInfoSemaphores.size()),
        .pSignalSemaphoreInfos = signalInfoSemaphores.data()
    };

    vkQueueSubmit2(graphicsQueue, 1, &submitInfo, nullptr);

    VkPresentInfoKHR presentInfo
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &data.renderCompletedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr
    };

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
    
}

void Application::Shutdown()
{
    vkDeviceWaitIdle(device);

    vmaDestroyBuffer(vmaAllocator, vertexBuffer.bufferHandle, vertexBuffer.allocation);
    vmaDestroyBuffer(vmaAllocator, indexBuffer.bufferHandle, indexBuffer.allocation);

    if(timelineSemaphore)
    {
        vkDestroySemaphore(device, timelineSemaphore, nullptr);
    }

    for (FrameData &data : frameData)
    {
        if(data.acquiredImageSemaphore)
        {
            vkDestroySemaphore(device, data.acquiredImageSemaphore, nullptr);
        }

        if(data.renderCompletedSemaphore)
        {
            vkDestroySemaphore(device, data.renderCompletedSemaphore, nullptr);
        }

        if(data.commandPool)
        {
            vkDestroyCommandPool(device, data.commandPool, nullptr);
        }
    }

    if(grapchisPipelineLayout)
    {
        vkDestroyPipelineLayout(device, grapchisPipelineLayout, nullptr);
    }

    if(graphicsPipeline)
    {
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
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
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    if(device)
    {
        vkDestroyDevice(device, nullptr);
    }

    if(instance)
    {
        vkDestroyInstance(instance, nullptr);
    }

    volkFinalize();

    if(window)
    {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
}

bool Application::InitializeVulkan()
{
    if(!CreateVulkanInstance())
    {
        return false;
    }

    if(!GetPhysicalDevice())
    {
        return false;
    }

    if(!CreateSurface())
    {
        return false;
    }

    if(!GetGraphicsQueue())
    {
        return false;
    }

    if(!CreateDevice())
    {
        return false;
    }

    if(!InitializeVMA())
    {
        return false;
    }

    if(!CreateSwapchain())
    {
        return false;
    }

    if(!CreateShaders())
    {
        return false;
    }

    CreateMeshBuffers();

    if(!CreateGraphicsPipeline())
    {
        return false;
    }

    if(!CreateSyncResource())
    {
        return false;
    }

    if(!CreateCommandBuffers())
    {
        return false;
    }

    return true;
}

bool Application::CreateVulkanInstance()
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
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    for (int i = 0; i < extensionsCount; i++)
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
        .pNext = &debugInfo,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(requestedLayers.size()),
        .ppEnabledLayerNames = requestedLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requestedExtensions.size()),
        .ppEnabledExtensionNames = requestedExtensions.data()
    };

    if(vkCreateInstance(&instanceCI, nullptr, &instance) != VK_SUCCESS)
    {
        printError("Unable to create the vulkan instance");
        return false;
    }
    
    volkLoadInstance(instance);

    return true;
}

bool Application::GetPhysicalDevice()
{
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());

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

    physicalDevice = choseGPU;

    return true;
}

bool Application::CreateSurface()
{
    if(!SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface))
    {
        printError("Unable to create the surface");
        return false;
    }

    return true;
}

bool Application::GetGraphicsQueue()
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties2> queueFamProps(count, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2});
    vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &count, queueFamProps.data());

    for (int i = 0; i < count; i++)
    {
        VkBool32 isPresentationSupported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &isPresentationSupported);

        if(queueFamProps[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && isPresentationSupported)
        {
            graphicsQueueFamIndex = i;
            return true;
        }
    }

    printError("Unable to get a graphics queue with required properties");

    return false;

}

bool Application::CreateDevice()
{
    #pragma region Features

    VkPhysicalDeviceVulkan14Features availableFeatures14 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES, .pNext = nullptr};
    VkPhysicalDeviceVulkan13Features availableFeatures13 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, .pNext = &availableFeatures14};
    VkPhysicalDeviceVulkan12Features availableFeatures12 {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &availableFeatures13};
    VkPhysicalDeviceFeatures2 availableFeatures {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &availableFeatures12};

    vkGetPhysicalDeviceFeatures2(physicalDevice, &availableFeatures);

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
        .queueFamilyIndex = graphicsQueueFamIndex,
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

    if(vkCreateDevice(physicalDevice, &deviceCI, nullptr, &device) != VK_SUCCESS)
    {
        printError("Unable to create the vulkan device");
        return false;
    }

    vkGetDeviceQueue(device, graphicsQueueFamIndex, 0, &graphicsQueue);
    if(!graphicsQueue)
    {
        printError("Unable to get the graphics queue");
        return false;
    }

    return true;
}

bool Application::InitializeVMA()
{
    VmaVulkanFunctions vmaFunctionsInfo {};
    VmaAllocatorCreateInfo allocatorCI
    {
        .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
        .physicalDevice = physicalDevice,
        .device = device,
        .pVulkanFunctions = &vmaFunctionsInfo,
        .instance = instance,
        .vulkanApiVersion = vulkanVersion
    };

    if(vmaImportVulkanFunctionsFromVolk(&allocatorCI, &vmaFunctionsInfo) != VK_SUCCESS)
    {
        printError("Unable to load Vulkan functions in VMA with Volk");
        return false;
    }

    if(vmaCreateAllocator(&allocatorCI, &vmaAllocator) != VK_SUCCESS)
    {
        printError("Unable to create the VMA allocator");
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
        printError("Unable to get surface capabilities");
        return false;
    }

    uint32_t swapchainImagesCount = std::max(2u, surfaceCaps.minImageCount);
    if(surfaceCaps.maxImageCount == 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatsCount, supportedFormats.data());

    bool isRequiredFormatSupported = false;
    for (const VkSurfaceFormatKHR format : supportedFormats)
    {
        if(format.format == swapchainFormat)
        {
            isRequiredFormatSupported = true;
            break;
        }
    }

    if(!isRequiredFormatSupported)
    {
        printError("The device does not support the required swapchain format");
    }

    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = swapchainImagesCount,
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
        printError("Unable to create the swapchain");
        return false;
    }

    // Swapchain images
    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, nullptr);
    swapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imagesCount, swapchainImages.data());

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
            printError("Unable to create swapchain image views");
            return false;
        }
    }

    // Depth images
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

    VmaAllocationCreateInfo vmaAllocInfo
    {
        .flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    frameData.resize(maxFramesInFlight);
    for(int i = 0; i < maxFramesInFlight; i++)
    {
        if(vmaCreateImage(vmaAllocator, &depthImageCI, &vmaAllocInfo, 
            &frameData[i].depthImage, &frameData[i].depthImageAllocation, nullptr) != VK_SUCCESS)
        {
            printError("Unable to create depth images");
            return false;
        }

        VkImageViewCreateInfo depthImageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = frameData[i].depthImage,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depthFormat,
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if(vkCreateImageView(device, &depthImageViewCI, nullptr, &frameData[i].depthImageView) != VK_SUCCESS)
        {
            printError("Unable to create depth image view");
            return false;
        }
    }

    return true;

}

bool Application::CreateShaders()
{
    vertexShader = CreateShaderModule("shader.vert", shaderc_vertex_shader);
    fragmentShader = CreateShaderModule("shader.frag", shaderc_fragment_shader);

    if(!vertexShader)
    {
        return false;
    }

    if(!fragmentShader)
    {
        return false;
    }

    return true;
}

bool Application::CreateGraphicsPipeline()
{
    VkPushConstantRange pushConstant
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(PushConstant)
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstant
    };

    if(vkCreatePipelineLayout(device, &pipelineLayoutCI, nullptr, &grapchisPipelineLayout) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline layout");
        return false;
    }

    // Shader stages info
    const char* entryPoint = "main";
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo
    {
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader,
            .pName = entryPoint
        },
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader,
            .pName = entryPoint
        }
    };

    VkVertexInputBindingDescription vertexBinding
    {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    std::array<VkVertexInputAttributeDescription, 2> vertexAttributes
    {
        VkVertexInputAttributeDescription
        {
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, position)
        },
        VkVertexInputAttributeDescription
        {
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, color)
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBinding,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size()),
        .pVertexAttributeDescriptions = vertexAttributes.data()
    };

    VkPipelineInputAssemblyStateCreateInfo inputAsseblyInfo
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

    VkPipelineRasterizationStateCreateInfo rasterizationInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_NONE,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1
    };

    VkPipelineMultisampleStateCreateInfo multisampleInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState colorAttachmentState
    {
       .blendEnable = VK_FALSE,
       .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentState
    };

    std::vector<VkDynamicState> dynamicStates
    {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
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
        .stageCount = static_cast<uint32_t>(shaderStagesInfo.size()),
        .pStages = shaderStagesInfo.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAsseblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = grapchisPipelineLayout
    };

    if(vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCI, nullptr, &graphicsPipeline) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline");
        return false;
    }

    return true;
}

bool Application::CreateSyncResource()
{

    // Timeline semaphore
    VkSemaphoreTypeCreateInfo timelineSemaphoreTypeCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = maxFramesInFlight
    };

    VkSemaphoreCreateInfo timelineSemaphoreCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &timelineSemaphoreTypeCI
    };

    if(vkCreateSemaphore(device, &timelineSemaphoreCI, nullptr, &timelineSemaphore) != VK_SUCCESS)
    {
        printError("Unable to create timeline semaphore");
        return false;
    }


    for(FrameData &data : frameData)
    {
        VkSemaphoreCreateInfo semaphoreCI
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        if(vkCreateSemaphore(device, &semaphoreCI, nullptr, &data.acquiredImageSemaphore) != VK_SUCCESS)
        {
            printError("Unable to create acquired image semaphore");
            return false;
        }

        if(vkCreateSemaphore(device, &semaphoreCI, nullptr, &data.renderCompletedSemaphore) != VK_SUCCESS)
        {
            printError("Unable to create render completed semaphore");
            return false;
        }
    }

    return true;
}

bool Application::CreateCommandBuffers()
{
    for (FrameData &data : frameData)
    {
        VkCommandPoolCreateInfo commandPoolCI
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = graphicsQueueFamIndex
        };

        if(vkCreateCommandPool(device, &commandPoolCI, nullptr, &data.commandPool) != VK_SUCCESS)
        {
            printError("Unable to create command pool");
            return false;
        }

        VkCommandBufferAllocateInfo commandBufferInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = data.commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if(vkAllocateCommandBuffers(device, &commandBufferInfo, &data.commandBuffer) != VK_SUCCESS)
        {
            printError("Unable to create the command buffer");
            return false;
        }
    }

    return true;
}

void Application::CreateMeshBuffers()
{
    VkBufferCreateInfo vertexBufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(Vertex) * vertices.size(),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VkBufferCreateInfo indexBufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(uint16_t) * indices.size(),
        .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo allocationCI
    {
        .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                 VMA_ALLOCATION_CREATE_MAPPED_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    if(vmaCreateBuffer(vmaAllocator, &vertexBufferCI, &allocationCI, 
        &vertexBuffer.bufferHandle, &vertexBuffer.allocation, &vertexBuffer.allocationInfo) != VK_SUCCESS)
    {
        printError("Unable to create vertex buffer");
    }

    if(vmaCreateBuffer(vmaAllocator, &indexBufferCI, &allocationCI, 
        &indexBuffer.bufferHandle, &indexBuffer.allocation, &indexBuffer.allocationInfo) != VK_SUCCESS)
    {
        printError("Unable to create index buffer");
    }

    memcpy(vertexBuffer.allocationInfo.pMappedData, vertices.data(), sizeof(Vertex) * vertices.size());
    memcpy(indexBuffer.allocationInfo.pMappedData, indices.data(), sizeof(uint16_t) * indices.size());

}

VkShaderModule Application::CreateShaderModule(std::string fileName, shaderc_shader_kind kind)
{
    const std::string path = "../source/shaders/" + fileName;
    std::string source = readTextFile(path);
    if(source.empty())
    {
        printError("Unable to find the shader '" + fileName + "' at path '" + path + "'");
        return nullptr;
    }

    shaderc::Compiler compiler;
    shaderc::CompileOptions compOpts;
    compOpts.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    compOpts.SetTargetSpirv(shaderc_spirv_version_1_6);
    compOpts.SetSourceLanguage(shaderc_source_language_glsl);
    compOpts.SetOptimizationLevel(shaderc_optimization_level_performance);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, fileName.c_str(), compOpts);

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        printError("Unable to compile the shader '" + fileName + "'");
        return nullptr;
    }

    const size_t size = (result.cend() - result.cbegin()) * sizeof(uint32_t);
    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = result.cbegin()
    };
    
    VkShaderModule shaderModule = nullptr;
    if(vkCreateShaderModule(device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Unable to create the shader module of the shader '" + fileName + "'");
        return nullptr;
    }

    return shaderModule;
}

void Application::DestroySwapchain()
{
    for (VkImageView &imageView : swapchainImageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapchainImageViews.clear();

    if(swapchain)
    {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
    swapchainImages.clear();
    swapchain = nullptr;

    for (FrameData &data : frameData)
    {
        if(data.depthImageView)
        {
            vkDestroyImageView(device, data.depthImageView, nullptr);
        }

        if(data.depthImage)
        {
            vmaDestroyImage(vmaAllocator, data.depthImage, data.depthImageAllocation);
        }

        data.depthImage = nullptr;
        data.depthImageView = nullptr;
        data.depthImageAllocation = nullptr;
    }
}