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

void Application::Run()
{
    isRunning = true;
	while (isRunning)
	{
		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				isRunning = false;
				break;
			}
			else if (event.type == SDL_EVENT_WINDOW_RESIZED)
			{
				windowWidth = event.window.data1;
				windowHeight = event.window.data2;
				break;
			}
		}

		Render();
	}
}

void Application::Render()
{
    if(isSwapchainRecreationRequired)
    {
        vkDeviceWaitIdle(device);
        DestroySwapchain();
        CreateSwapchain();
        isSwapchainRecreationRequired = false;
    }

    const uint32_t frameResIndex = frameIndex++ % maxFramesInFlight;
    const uint64_t signalValue = nextSignalValue++;
    const uint64_t waitValue = signalValue - maxFramesInFlight;

    VkSemaphoreWaitInfo waitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .semaphoreCount = 1,
        .pSemaphores = &timelineSemaphore,
        .pValues = &waitValue
    };
    vkWaitSemaphores(device, &waitInfo, UINT64_MAX);

    FrameResources &frameResource = frameResources[frameResIndex];
    vkResetCommandPool(device, frameResource.commandPool, 0);

    VkSemaphore imageAcquiredSemaphore = frameResource.imageAcquiredSemaphore;

    uint32_t imageIndex = 0;
    VkResult acquireResult = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquiredSemaphore, 
                                            VK_NULL_HANDLE, &imageIndex);

    if(acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        isSwapchainRecreationRequired = true;
        return;
    }
    else if(acquireResult == VK_SUBOPTIMAL_KHR)
    {
        isSwapchainRecreationRequired = true;
    }

    VkCommandBufferBeginInfo cmdBeginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    vkBeginCommandBuffer(frameResource.commandBuffer, &cmdBeginInfo);

    std::vector<VkImageMemoryBarrier2> layoutBarriers
    {
        VkImageMemoryBarrier2
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
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
            .image = depthImage,
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

    VkDependencyInfo depInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32_t>(layoutBarriers.size()),
        .pImageMemoryBarriers = layoutBarriers.data()
    };

    vkCmdPipelineBarrier2(frameResource.commandBuffer, &depInfo);

    VkRenderingAttachmentInfo colorAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = swapchainImageViews[frameResIndex],
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue {.color{{0, 0, 0, 1}}}
    };

    VkRenderingAttachmentInfo depthAttachmentInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView = depthImageView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue {.depthStencil{1.0f, 0}}
    };

    VkRenderingInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea
        {
            .offset {0,0},
            .extent {.width = swapchainWidth, .height = swapchainHeight}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthAttachmentInfo
    };

    vkCmdBeginRendering(frameResource.commandBuffer, &renderInfo);
    {
        VkViewport viewport 
        {
            .x = 0, .y = 0,
            .width = static_cast<float>(swapchainWidth),
            .height = static_cast<float>(swapchainWidth)
        };
        vkCmdSetViewport(frameResource.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = swapchainWidth, .height = swapchainHeight}
        };
        vkCmdSetScissor(frameResource.commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(frameResource.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(frameResource.commandBuffer, 3, 1, 0, 0);

    }
    vkCmdEndRendering(frameResource.commandBuffer);

    VkImageMemoryBarrier2 presentLayoutBarrier
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

    VkDependencyInfo presentDependencyInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &presentLayoutBarrier
    };
    vkCmdPipelineBarrier2(frameResource.commandBuffer, &presentDependencyInfo);

    vkEndCommandBuffer(frameResource.commandBuffer);

    VkSemaphoreSubmitInfo imageAcquireWaitInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .semaphore = imageAcquiredSemaphore,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    std::vector<VkSemaphoreSubmitInfo> semaphoreSignals
	{
		{ // render work completion signal
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = renderCompleteSemaphores[imageIndex],
			.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
		},
		{ // entire frame is completed (timeline)
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = timelineSemaphore,
			.value = signalValue,
			.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT
		}
	};
	VkCommandBufferSubmitInfo cmdSubmitInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
		.commandBuffer = frameResource.commandBuffer,
	};
	VkSubmitInfo2 submitInfo
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
		.waitSemaphoreInfoCount = 1,
		.pWaitSemaphoreInfos = &imageAcquireWaitInfo, // ensure the image is ready
		.commandBufferInfoCount = 1,
		.pCommandBufferInfos = &cmdSubmitInfo,
		.signalSemaphoreInfoCount = static_cast<uint32_t>(semaphoreSignals.size()),
		.pSignalSemaphoreInfos = semaphoreSignals.data()
	};
	vkQueueSubmit2(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

	// present the image
	VkPresentInfoKHR presentInfo{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &renderCompleteSemaphores[imageIndex], // render work completed semaphore
		.swapchainCount = 1,
		.pSwapchains = &swapchain,
		.pImageIndices = &imageIndex,
		.pResults = nullptr
	};

	vkQueuePresentKHR(graphicsQueue, &presentInfo);

}

#pragma region Vulkan Initialization

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

    if(!CreateSyncResources())
    {
        printError("Could not create sync resources");
        return false;
    }

    if(!CreateCommandBuffers())
    {
        printError("Unable to create command buffer objects");
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

bool Application::CreateSyncResources()
{
    VkSemaphoreTypeCreateInfo semaphoreTypeInfo
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = maxFramesInFlight
    };

    VkSemaphoreCreateInfo timelineSemaphoreCI
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &semaphoreTypeInfo
    };

    if(vkCreateSemaphore(device, &timelineSemaphoreCI, nullptr, &timelineSemaphore) != VK_SUCCESS)
    {
        printError("Could not create the timeline semaphore");
        return false;
    }

    for (FrameResources &frameResource : frameResources)
    {
        VkSemaphoreCreateInfo semaphoreCI {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        if(vkCreateSemaphore(device, &semaphoreCI, nullptr, &frameResource.imageAcquiredSemaphore) != VK_SUCCESS)
        {
            printError("Could not create per-frame image-acquire semaphore");
            return false;
        }
    }

    return true;
}

bool Application::CreateCommandBuffers()
{
    for (FrameResources &frameResource : frameResources)
    {
        VkCommandPoolCreateInfo commandPoolCI
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = graphicsQueueFamilyIndex
        };

        if(vkCreateCommandPool(device, &commandPoolCI, nullptr, &frameResource.commandPool) != VK_SUCCESS)
        {
            printError("Unable to create the command pool");
            return false;
        }

        VkCommandBufferAllocateInfo cmdAllocateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = frameResource.commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        if(vkAllocateCommandBuffers(device, &cmdAllocateInfo, &frameResource.commandBuffer) != VK_SUCCESS)
        {
            printError("Unable to create the command buffer");
            return false;
        }
    }

    return true;
}

#pragma endregion

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
    vkDeviceWaitIdle(device);

    if(timelineSemaphore)
    {
        vkDestroySemaphore(device, timelineSemaphore, nullptr);
    }

    for (FrameResources &frameResource : frameResources)
    {
        vkDestroySemaphore(device, frameResource.imageAcquiredSemaphore, nullptr);
        vkDestroyCommandPool(device, frameResource.commandPool, nullptr);
        // No need to destroy command buffers because the are implicitly destroyed by 'vkDestroyCommandPool'
    }

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