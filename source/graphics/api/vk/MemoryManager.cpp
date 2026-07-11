#include "Eve/graphics/Sampler.hpp"
#include <graphics/api/vk/MemoryManager.hpp>

using namespace Eve::Graphics;

ImageHandle MemoryManager::AllocateImage(ImageInfo imageInfo)
{
    Image image;

    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = GetVkImageType(imageInfo.Type),
        .format = GetVkImageFormat(imageInfo.Format),
        .extent {.width = imageInfo.Width, .height = imageInfo.Height, .depth = imageInfo.Depth},
        .mipLevels = imageInfo.MipLevels,
        .arrayLayers = imageInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(imageInfo.SampleCount),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(imageInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo imageAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateImage(ContextBuilder::context.Allocator, &imageCI, &imageAllocInfo, 
        &image.Image, &image.Allocation, &image.AllocationInfo);
    
    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.Image,
        .viewType = GetVkImageViewType(imageInfo.ViewType),
        .format = GetVkImageFormat(imageInfo.Format),
        .subresourceRange
        {
            .aspectMask = GetVkImageAspectMask(imageInfo.AspectMask),
            .baseMipLevel = 0,
            .levelCount = imageInfo.MipLevels,
            .baseArrayLayer = 0,
            .layerCount = imageInfo.ArrayLayers
        }
    };

    vkCreateImageView(ContextBuilder::context.Device, &imageViewCI, nullptr, &image.ImageView);

    ImageHandle handle;
    if(imageFreeSlots.empty()) 
    {
        handle.Id = images.size();

        images.push_back(image);
    }
    else 
    {
        handle.Id = imageFreeSlots.back();
        imageFreeSlots.pop_back();

        images[handle.Id] = image;
    }

    return handle;
}

SamplerHandle MemoryManager::AllocateSampler(SamplerInfo samplerInfo)
{
    Sampler sampler;

    VkSamplerCreateInfo samplerCI
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = GetVkFilterMode(samplerInfo.MagFilter),
        .minFilter = GetVkFilterMode(samplerInfo.MinFilter),
        .mipmapMode = GetVkMipmapMode(samplerInfo.MipmapMode),
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(ContextBuilder::context.Device, &samplerCI, nullptr, &sampler.Sampler);

    SamplerHandle handle;
    if(samplerFreeSlots.empty()) 
    {
        handle.Id = samplers.size();

        samplers.push_back(sampler);
    }
    else 
    {
        handle.Id = samplerFreeSlots.back();
        samplerFreeSlots.pop_back();

        samplers[handle.Id] = sampler;
    }

    return handle;
}

BufferHandle MemoryManager::AllocateBuffer(BufferInfo bufferInfo)
{
    Buffer buffer;

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferInfo.Size,
        .usage = GetVkBufferType(bufferInfo.Type) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo bufferAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateBuffer(ContextBuilder::context.Allocator, &bufferCI, &bufferAllocInfo,
         &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo);

    BufferHandle handle;
    if(bufferFreeSlots.empty()) 
    {
        handle.Id = images.size();

        buffers.push_back(buffer);
    }
    else 
    {
        handle.Id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        buffers[handle.Id] = buffer;
    }

    return handle;
}