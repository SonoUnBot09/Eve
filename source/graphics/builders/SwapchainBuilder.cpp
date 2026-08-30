#include <graphics/GraphicsCore.hpp>
#include "SwapchainBuilder.hpp"
#include "eve/graphics/Sampler.hpp"
#include "graphics/registers/MemoryRegistry.hpp"
#include <graphics/MemoryBin.hpp>
#include <graphics/registers/ShaderRegistry.hpp>
#include <graphics/ErrorManager.hpp>

#include <EveSettings.hpp>
#include <eve/debug/Debug.hpp>

using namespace Eve::Debug;
using namespace Eve::Graphics;

bool SwapchainBuilder::Build(Swapchain& swapchain)
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, 
        &surfaceCaps));

    if(surfaceCaps.currentExtent.width != 0xFFFFFFFF)
    {
        swapchain.Width = surfaceCaps.currentExtent.width;
        swapchain.Height = surfaceCaps.currentExtent.height;
    }
    else 
    {
        int w, h;
        SDL_GetWindowSizeInPixels(GraphicsCore::Window.Window, &w, &h);

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

    uint32_t swapchainImagesCount = 3; 

    if(surfaceCaps.minImageCount > swapchainImagesCount)
    {
        swapchainImagesCount = surfaceCaps.minImageCount;
    }
    
    if(surfaceCaps.maxImageCount != 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, 
        &supportedFormatsCount, nullptr));
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, 
        &supportedFormatsCount, supportedFormats.data()));

    bool isRequiredFormatSupported = false;
    // Fallback for any format supported
    if(supportedFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        isRequiredFormatSupported = true;
        chooseFormat = Eve::Settings::swapchainFormats[0];
    }

    for (const VkFormat proposedFormat : Eve::Settings::swapchainFormats)
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
        return false;
    }

    swapchain.Format = chooseFormat;

    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = GraphicsCore::Context.Surface,
        .minImageCount = swapchainImagesCount,
        .imageFormat = chooseFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent {.width = swapchain.Width, .height = swapchain.Height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    VK_CHECK(vkCreateSwapchainKHR(GraphicsCore::Context.Device, &swapchainCI, nullptr, &swapchain.Swapchain));

    // Swapchain images
    uint32_t imagesCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, nullptr));
    swapchain.swapchainImages.resize(imagesCount);
    VK_CHECK(vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, swapchain.swapchainImages.data()));

    swapchain.swapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain.swapchainImages[i],
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

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &swapchain.swapchainImageViews[i]));
    }

    Format format = swapchain.Format == VK_FORMAT_R8G8B8A8_SRGB ? Format::FORMAT_R8G8B8A8_SRGB : Format::FORMAT_B8G8R8A8_SRGB;
    
    ShaderInfo shaderInfo
    {
        .ShaderModule = "swapchain",
        .Topology = Topology::TOPOLOGY_TRIANGLE_LIST,
        .PolygonMode = PolygonMode::POLYGON_MODE_FILL,
        .CullMode = CullMode::CULL_MODE_NONE,
        .LineWidth = 1,
        .DepthTest = false,
        .DepthWrite = false,
        .StencilTest = false,
        .CompareOp = DepthTest::DEPTH_COMPARE_LESS,
        .ColorFormat = format
    };

    swapchain.shader = ShaderRegistry::CreateGraphicsShader(shaderInfo);

    SamplerInfo samplerInfo
    {
        .MinFilter = Filter::FILTER_LINEAR,
        .MagFilter = Filter::FILTER_LINEAR,
        .MipmapMode = MipmapMode::MIPMAP_LINEAR
    };

    swapchain.sampler = MemoryRegistry::CreateSampler(samplerInfo);

    return true;
}

bool SwapchainBuilder::Rebuild(Swapchain& swapchain)
{    
    Destroy(swapchain);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, &surfaceCaps));

    if(surfaceCaps.currentExtent.width != 0xFFFFFFFF)
    {
        swapchain.Width = surfaceCaps.currentExtent.width;
        swapchain.Height = surfaceCaps.currentExtent.height;
    }
    else 
    {
        int w, h;
        SDL_GetWindowSizeInPixels(GraphicsCore::Window.Window, &w, &h);

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

    uint32_t swapchainImagesCount = 3; 

    if(surfaceCaps.minImageCount > swapchainImagesCount)
    {
        swapchainImagesCount = surfaceCaps.minImageCount;
    }
    
    if(surfaceCaps.maxImageCount != 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, 
        &supportedFormatsCount, nullptr));
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, 
        &supportedFormatsCount, supportedFormats.data()));

    bool isRequiredFormatSupported = false;
    // Fallback for any format supported
    if(supportedFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        isRequiredFormatSupported = true;
        chooseFormat = Eve::Settings::swapchainFormats[0];
    }

    for (const VkFormat proposedFormat : Eve::Settings::swapchainFormats)
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
        return false;
    }

    swapchain.Format = chooseFormat;

    VkSwapchainCreateInfoKHR swapchainCI
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = GraphicsCore::Context.Surface,
        .minImageCount = swapchainImagesCount,
        .imageFormat = chooseFormat,
        .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        .imageExtent {.width = swapchain.Width, .height = swapchain.Height},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = surfaceCaps.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR
    };

    VK_CHECK(vkCreateSwapchainKHR(GraphicsCore::Context.Device, &swapchainCI, nullptr, &swapchain.Swapchain));

    // Swapchain images
    uint32_t imagesCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, nullptr));
    swapchain.swapchainImages.resize(imagesCount);
    VK_CHECK(vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, swapchain.swapchainImages.data()));

    swapchain.swapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain.swapchainImages[i],
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

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &swapchain.swapchainImageViews[i]));
    }

    return true;
}

void SwapchainBuilder::Destroy(Swapchain& swapchain)
{
    for (uint32_t i = 0; i < swapchain.swapchainImages.size(); i++)
    {
        swapchain.swapchainImages[i] = nullptr;
        vkDestroyImageView(GraphicsCore::Context.Device, swapchain.swapchainImageViews[i], nullptr);

    }
    swapchain.swapchainImages.clear();
    swapchain.swapchainImageViews.clear();

    if(swapchain.Swapchain)
    {
        vkDestroySwapchainKHR(GraphicsCore::Context.Device, swapchain.Swapchain, nullptr);
    }
}