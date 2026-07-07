#include <graphics/api/vk/SwapchainBuilder.hpp>

#include <vulkan/vulkan.hpp>

#include <Eve/Debug.hpp>
using namespace Debug;

Swapchain SwapchainBuilder::Build(Context& context, Window& window, bool& success)
{

    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.PhysicalDevice, context.Surface, &surfaceCaps) != VK_SUCCESS)
    {
        printError("Unable to get surface capabilities");
        success = false;
        return swapchain;
    }

    if(surfaceCaps.currentExtent.width != 0xFFFFFFFF)
    {
        swapchain.Width = surfaceCaps.currentExtent.width;
        swapchain.Height = surfaceCaps.currentExtent.height;
    }
    else 
    {
        int w, h;
        SDL_GetWindowSizeInPixels(window.Window, &w, &h);

        uint32_t width = static_cast<uint32_t>(w);
        uint32_t height = static_cast<uint32_t>(h);

        swapchain.Width = std::clamp(
            width, 
            surfaceCaps.minImageExtent.width,
            surfaceCaps.maxImageExtent.width);

        swapchain.Height = std::clamp(
            height,
            surfaceCaps.minImageExtent.height,
            surfaceCaps.maxImageExtent.height
        );
        
    }

    swapchainImagesCount = std::max(2u, surfaceCaps.minImageCount);
    if(surfaceCaps.maxImageCount == 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.PhysicalDevice, context.Surface, &supportedFormatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.PhysicalDevice, context.Surface, &supportedFormatsCount, supportedFormats.data());

    bool isRequiredFormatSupported = false;
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

    swapchain.Format = chooseFormat;

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
        success = false;
        return swapchain;
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
            success = false;
            return swapchain;
        }
    }

    success = true;
    return swapchain;
}

Swapchain SwapchainBuilder::Rebuild(Context& context, Window& window, bool& success)
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.PhysicalDevice, context.Surface, &surfaceCaps) != VK_SUCCESS)
    {
        printError("Unable to get surface capabilities");
        success = false;
        return swapchain;
    }

    if(surfaceCaps.currentExtent.width != 0xFFFFFFFF)
    {
        swapchain.Width = surfaceCaps.currentExtent.width;
        swapchain.Height = surfaceCaps.currentExtent.height;
    }
    else 
    {
        int w, h;
        SDL_GetWindowSizeInPixels(window.Window, &w, &h);

        uint32_t width = static_cast<uint32_t>(w);
        uint32_t height = static_cast<uint32_t>(h);

        swapchain.Width = std::clamp(
            width, 
            surfaceCaps.minImageExtent.width,
            surfaceCaps.maxImageExtent.width);

        swapchain.Height = std::clamp(
            height,
            surfaceCaps.minImageExtent.height,
            surfaceCaps.maxImageExtent.height
        );
        
    }

    swapchain.Format = chooseFormat;

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
        success = false;
        return swapchain;
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
            success = false;
            return swapchain;
        }
    }

    success = true;
    return swapchain;
}