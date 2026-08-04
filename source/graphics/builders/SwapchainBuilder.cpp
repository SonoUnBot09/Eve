#include <graphics/GraphicsCore.hpp>
#include "SwapchainBuilder.hpp"
#include "Eve/graphics/Geometry.hpp"
#include "graphics/Resources.hpp"
#include "graphics/registers/MemoryRegistry.hpp"
#include "graphics/registers/ShaderRegistry.hpp"
#include <graphics/MemoryBin.hpp>

#include <EveSettings.hpp>
#include <Eve/Debug.hpp>

using namespace Debug;
using namespace Eve::Graphics;

bool SwapchainBuilder::Build(Swapchain& swapchain)
{
    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, &surfaceCaps) != VK_SUCCESS)
    {
        printError("Unable to get surface capabilities");
        return false;
    }

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

    swapchainImagesCount = std::max(2u, surfaceCaps.minImageCount);
    if(surfaceCaps.maxImageCount == 0)
    {
        swapchainImagesCount = std::min(swapchainImagesCount, surfaceCaps.maxImageCount);
    }

    uint32_t supportedFormatsCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, &supportedFormatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> supportedFormats(supportedFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, &supportedFormatsCount, supportedFormats.data());

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
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR
    };

    if(vkCreateSwapchainKHR(GraphicsCore::Context.Device, &swapchainCI, nullptr, &swapchain.Swapchain) != VK_SUCCESS)
    {
        printError("Unable to create the swapchain");
        return false;
    }

    // Swapchain images
    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, nullptr);
    swapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, swapchainImages.data());

   swapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
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

        if(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            printError("Unable to create swapchain image views");
            return false;
        }
    }

    Format format = swapchain.Format == VK_FORMAT_R8G8B8A8_SRGB ? Format::FORMAT_R8G8B8A8_SRGB : Format::FORMAT_B8G8R8A8_SRGB;

    for(uint32_t i = 0; i < imagesCount; i++)
    {
        TextureObject texture
        {
            .Image = swapchainImages[i],
            .ImageView = swapchainImageViews[i]
        };

        TextureInfo textureInfo
        {
            .Data.TextureType = TextureType::TEXTURE_2D,
            .Data.Width = swapchain.Width,
            .Data.Height = swapchain.Height,
            .Data.Depth = 1,
            .Data.ArrayLayers = 1,
            .Data.Format = format,
            .Data.MipLevels = 1
        };

        TextureHandle handle = MemoryRegistry::ReserveTextureSlot2D(texture, textureInfo);

        swapchain.swapchainImagesHandles.push_back(handle);
    }

    // TODO: Add the real shader path
    
    ShaderInfo shaderInfo
    {
        .VertOffset = 0,
        .VertStride = 0,
        .FragOffset = 0,
        .FragStride = 0,
        .Topology = Topology::TOPOLOGY_TRIANGLE_LIST,
        .PolygonMode = PolygonMode::POLYGON_MODE_FILL,
        .CullMode = CullMode::CULL_MODE_NONE,
        .LineWidth = 1,
        .DepthTest = false,
        .DepthWrite = false,
        .StencilTest = false,
        .CompareOp = DepthTest::DEPTH_COMPARE_LESS,
        .samplesCount = TextureSample::SAMPLE_1,
        .ColorFormat = format
    };

    swapchain.shader = ShaderRegistry::CreateGraphicsShader(shaderInfo);

    return true;
}

bool SwapchainBuilder::Rebuild(Swapchain& swapchain)
{
    vkDeviceWaitIdle(GraphicsCore::Context.Device);

    MemoryBin::MarkGPUAsIdle();
    
    Destroy(swapchain);

    VkSurfaceCapabilitiesKHR surfaceCaps;
    if(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsCore::Context.PhysicalDevice, GraphicsCore::Context.Surface, &surfaceCaps) != VK_SUCCESS)
    {
        printError("Unable to get surface capabilities");
        return false;
    }

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
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR
    };

    if(vkCreateSwapchainKHR(GraphicsCore::Context.Device, &swapchainCI, nullptr, &swapchain.Swapchain) != VK_SUCCESS)
    {
        printError("Unable to create the swapchain");
        return false;
    }

    // Swapchain images
    uint32_t imagesCount = 0;
    vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, nullptr);
    swapchainImages.resize(imagesCount);
    vkGetSwapchainImagesKHR(GraphicsCore::Context.Device, swapchain.Swapchain, &imagesCount, swapchainImages.data());

    swapchainImageViews.resize(imagesCount);
    for(int i = 0; i < imagesCount; i++)
    {
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchainImages[i],
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

        if(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
        {
            printError("Unable to create swapchain image views");
            return false;
        }
    }

    Format format = swapchain.Format == VK_FORMAT_R8G8B8A8_SRGB ? Format::FORMAT_R8G8B8A8_SRGB : Format::FORMAT_B8G8R8A8_SRGB;

    for(uint32_t i = 0; i < imagesCount; i++)
    {
        TextureObject texture
        {
            .Image = swapchainImages[i],
            .ImageView = swapchainImageViews[i]
        };

        TextureInfo textureInfo
        {
            .Data.TextureType = TextureType::TEXTURE_2D,
            .Data.Width = swapchain.Width,
            .Data.Height = swapchain.Height,
            .Data.Depth = 1,
            .Data.ArrayLayers = 1,
            .Data.Format = format,
            .Data.MipLevels = 1
        };
        TextureHandle handle = MemoryRegistry::ReserveTextureSlot2D(texture, textureInfo);

        swapchain.swapchainImagesHandles.push_back(handle);
    }

    return true;
}

void SwapchainBuilder::Destroy(Swapchain& swapchain)
{
    for (TextureHandle handle : swapchain.swapchainImagesHandles)
    {
        TextureObject& texture = MemoryRegistry::GetTexture(handle);
        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);

        MemoryRegistry::FreeTextureSlot(handle);
    }
    swapchain.swapchainImagesHandles.clear();

    if(swapchain.Swapchain)
    {
        vkDestroySwapchainKHR(GraphicsCore::Context.Device, swapchain.Swapchain, nullptr);
    }
}