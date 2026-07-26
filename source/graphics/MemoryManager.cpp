#include "MemoryManager.hpp"
#include "ResourceMapper.hpp"
#include "GraphicsCore.hpp"

using namespace Eve::Graphics;

TextureHandle MemoryManager::AllocateTexture1D(TextureInfo1D textureInfo)
{
    Texture image;

    VkFormat format = GetVkImageFormat(textureInfo.Format);
    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType =VK_IMAGE_TYPE_1D,
        .format = format,
        .extent {.width = textureInfo.Width, .height = 1, .depth = 1},
        .mipLevels = textureInfo.MipLevels,
        .arrayLayers = textureInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(textureInfo.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(textureInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo imageAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
        &image.Image, &image.Allocation, &image.AllocationInfo);
    
    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.Image,
        .viewType = VK_IMAGE_VIEW_TYPE_1D,
        .format = format,
        .subresourceRange
        {
            .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
            .baseMipLevel = 0,
            .levelCount = textureInfo.MipLevels,
            .baseArrayLayer = 0,
            .layerCount = textureInfo.ArrayLayers
        }
    };

    vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &image.ImageView);

    TextureHandle handle = ReserveTextureSlot(image);

    return handle;
}

TextureHandle MemoryManager::AllocateTexture2D(TextureInfo2D textureInfo)
{
    Texture image;

    VkFormat format = GetVkImageFormat(textureInfo.Format);
    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent {.width = textureInfo.Width, .height = textureInfo.Width, .depth = 1},
        .mipLevels = textureInfo.MipLevels,
        .arrayLayers = textureInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(textureInfo.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(textureInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo imageAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
        &image.Image, &image.Allocation, &image.AllocationInfo);
    
    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.Image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange
        {
            .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
            .baseMipLevel = 0,
            .levelCount = textureInfo.MipLevels,
            .baseArrayLayer = 0,
            .layerCount = textureInfo.ArrayLayers
        }
    };

    vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &image.ImageView);

    TextureHandle handle = ReserveTextureSlot(image);

    return handle;
}

TextureHandle MemoryManager::AllocateTexture3D(TextureInfo3D textureInfo)
{
    Texture image;

    VkFormat format = GetVkImageFormat(textureInfo.Format);
    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_3D,
        .format = format,
        .extent {.width = textureInfo.Width, .height = textureInfo.Width, .depth = textureInfo.Depth},
        .mipLevels = textureInfo.MipLevels,
        .arrayLayers = textureInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(textureInfo.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(textureInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo imageAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateImage(GraphicsCore::Context.Allocator, &imageCI, &imageAllocInfo, 
        &image.Image, &image.Allocation, &image.AllocationInfo);
    
    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.Image,
        .viewType = VK_IMAGE_VIEW_TYPE_3D,
        .format = format,
        .subresourceRange
        {
            .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
            .baseMipLevel = 0,
            .levelCount = textureInfo.MipLevels,
            .baseArrayLayer = 0,
            .layerCount = textureInfo.ArrayLayers
        }
    };

    vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &image.ImageView);

    TextureHandle handle = ReserveTextureSlot(image);

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

    vkCreateSampler(GraphicsCore::Context.Device, &samplerCI, nullptr, &sampler.Sampler);

    SamplerHandle handle = ReserveSamplerSlot(sampler);

    return handle;
}

BufferHandle MemoryManager::AllocateBuffer(BufferInfo bufferInfo)
{
    Buffer buffer;

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferInfo.Data.Size,
        .usage = GetVkBufferUsage(bufferInfo.Data.Usage) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo bufferAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateBuffer(GraphicsCore::Context.Allocator, &bufferCI, &bufferAllocInfo,
         &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo);

    BufferHandle handle = ReserveBufferSlot(buffer);

    return handle;
}

BufferHandle MemoryManager::AllocateHostBuffer(BufferInfo bufferInfo)
{
    Buffer buffer;

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferInfo.Data.Size,
        .usage = GetVkBufferUsage(bufferInfo.Data.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo bufferAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST
    };

    vmaCreateBuffer(GraphicsCore::Context.Allocator, &bufferCI, &bufferAllocInfo,
         &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo);

    BufferHandle handle;
    if(bufferFreeSlots.empty()) 
    {
        handle.Id = buffers.size();

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

VmaPool MemoryManager::AllocateMemoryPool(uint32_t size)
{

    VmaPoolCreateInfo poolCI
    {
        .blockSize = size,
        .minBlockCount = 1,
        .maxBlockCount = 1,
        .priority = 1
    };
    VmaPool pool;

    vmaCreatePool(GraphicsCore::Context.Allocator, &poolCI, &pool);

    return pool;
}

TextureHandle MemoryManager::ReserveTextureSlot(Texture& texture)
{
    TextureHandle handle;
    if(imageFreeSlots.empty()) 
    {
        handle.Id = textures.size();

        textures.push_back(texture);
    }
    else 
    {
        handle.Id = imageFreeSlots.back();
        imageFreeSlots.pop_back();

        textures[handle.Id] = texture;
    }

    ResourceMapper::ScheduleImageMapping(handle);

    return handle;
}

SamplerHandle MemoryManager::ReserveSamplerSlot(Sampler sampler)
{
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

    ResourceMapper::ScheduleSamplerMapping(handle);

    return handle;
}

BufferHandle MemoryManager::ReserveBufferSlot(Buffer& buffer)
{
    BufferHandle handle;
    if(bufferFreeSlots.empty()) 
    {
        handle.Id = buffers.size();

        buffers.push_back(buffer);
    }
    else 
    {
        handle.Id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        buffers[handle.Id] = buffer;
    }

    ResourceMapper::ScheduleBufferMapping(handle);

    return handle;
}

TransientTextureHandle MemoryManager::ReserveTransientTextureSlot()
{
    TextureHandle handle;
    Texture texture {0, 0, 0, 0};
    if(imageFreeSlots.empty()) 
    {
        handle.Id = textures.size();

        textures.push_back(texture);
    }
    else 
    {
        handle.Id = imageFreeSlots.back();
        imageFreeSlots.pop_back();

        textures[handle.Id] = texture;
    }

    ResourceMapper::ScheduleImageMapping(handle);

    return TransientTextureHandle{handle.Id};
}

TransientBufferHandle MemoryManager::ReserveTransientBufferSlot()
{
    BufferHandle handle;
    Buffer buffer{0,0,0};
    if(bufferFreeSlots.empty()) 
    {
        handle.Id = buffers.size();

        buffers.push_back(buffer);
    }
    else 
    {
        handle.Id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        buffers[handle.Id] = buffer;
    }

    ResourceMapper::ScheduleBufferMapping(handle);

    return TransientBufferHandle{handle.Id};
}