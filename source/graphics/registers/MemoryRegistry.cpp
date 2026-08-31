#include "MemoryRegistry.hpp"
#include <cstdint>
#include <eve/graphics/Buffer.hpp>
#include <eve/graphics/Pass.hpp>
#include <eve/graphics/Texture.hpp>
#include "ResourceRegistry.hpp"
#include <graphics/ResourceMapper.hpp>
#include <graphics/GraphicsCore.hpp>
#include <graphics/MemoryBin.hpp>
#include <graphics/Resources.hpp>
#include <graphics/helpers/AllocationHelper.hpp>
#include <graphics/helpers/VulkanMapping.hpp>
#include <graphics/RenderGraph.hpp>
#include "ResourceRegistry.hpp"

using namespace Eve::Graphics;

TextureHandle MemoryRegistry::CreateTexture1D(TextureInfo1D textureInfo)
{
    VkImageUsageFlags usage =   VK_IMAGE_USAGE_SAMPLED_BIT | 
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if(textureInfo.randomReadWrite)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }          

    TextureObject texture {};
    Helpers::AllocateTexture1D(textureInfo, texture, usage);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_1D,
        .Width = textureInfo.Width,
        .Height = 1,
        .Depth = 1,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, texture.ImageView, info);

    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture2D(TextureInfo2D textureInfo)
{
    VkImageUsageFlags usage =   VK_IMAGE_USAGE_SAMPLED_BIT | 
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if(textureInfo.randomReadWrite)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }     

    TextureObject texture {};
    Helpers::AllocateTexture2D(textureInfo, texture, usage);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_2D,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle,  texture.ImageView, info);

    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture3D(TextureInfo3D textureInfo)
{
    VkImageUsageFlags usage =   VK_IMAGE_USAGE_SAMPLED_BIT | 
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if(textureInfo.randomReadWrite)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }     

    TextureObject texture {};
    Helpers::AllocateTexture3D(textureInfo, texture, usage);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_3D,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = textureInfo.Depth,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, texture.ImageView, info);


    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTextureCube(TextureInfo2D textureInfo)
{
    VkImageUsageFlags usage =   VK_IMAGE_USAGE_SAMPLED_BIT | 
                                VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if(textureInfo.randomReadWrite)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }     

    TextureObject texture {};
    Helpers::AllocateTextureCube(textureInfo, texture, usage);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_CUBE,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, texture.ImageView, info);


    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

SamplerHandle MemoryRegistry::CreateSampler(SamplerInfo samplerInfo)
{
    SamplerObject sampler {};
    Helpers::AllocateSampler(samplerInfo, sampler);

    SamplerInfo info
    {
        .MinFilter = samplerInfo.MinFilter,
        .MagFilter = samplerInfo.MagFilter,
        .MipmapMode = samplerInfo.MipmapMode
    };

    SamplerHandle handle = ResourceRegistry::RequestPersistentSamplerSlot();

    ResourceMapper::ScheduleSamplerMapping(handle, sampler.Sampler);

    samplers.resize(ResourceRegistry::samplerResourcesPeakIndex);
    samplersInfo.resize(ResourceRegistry::samplerResourcesPeakIndex);

    samplers[handle.Id] = sampler;
    samplersInfo[handle.Id] = info;

    return handle;
}

BufferHandle MemoryRegistry::CreateGPUBuffer(uint64_t size)
{
    BufferInfo info
    {
        .Size = size,
        .Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
    };

    BufferObject buffer {};
    Helpers::AllocateGPUBuffer(info, buffer);

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();

    ResourceMapper::ScheduleBufferMapping(handle, buffer.Buffer);

    buffers.resize(ResourceRegistry::bufferResourcesPeakIndex);
    buffersInfo.resize(ResourceRegistry::bufferResourcesPeakIndex);

    buffers[handle.Id] = buffer;
    buffersInfo[handle.Id] = info;

    return handle;
}

BufferHandle MemoryRegistry::CreateUBOBuffer(uint64_t size)
{
    BufferInfo info
    {
        .Size = size,
        .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
    };

    BufferObject buffer {};
    Helpers::AllocateGPUBuffer(info, buffer);

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();

    ResourceMapper::ScheduleBufferMapping(handle, buffer.Buffer);

    buffers.resize(ResourceRegistry::bufferResourcesPeakIndex);
    buffersInfo.resize(ResourceRegistry::bufferResourcesPeakIndex);

    buffers[handle.Id] = buffer;
    buffersInfo[handle.Id] = info;

    return handle;
}
BufferHandle MemoryRegistry::CreateCPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateCPUBuffer(bufferInfo, buffer);

    BufferInfo info
    {
        .Size = bufferInfo.Size,
        .Usage = bufferInfo.Usage
    };

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();

    buffers.resize(ResourceRegistry::bufferResourcesPeakIndex);
    buffersInfo.resize(ResourceRegistry::bufferResourcesPeakIndex);

    buffers[handle.Id] = buffer;
    buffersInfo[handle.Id] = info;

    return handle;
}

void MemoryRegistry::DestroyBuffer(BufferHandle handle)
{
    BufferObject buffer = buffers[handle.Id];

    MemoryBin::DestroyPersistentBuffer(buffer);

    ResourceRegistry::FreePersistentBufferSlot(handle);
}

void MemoryRegistry::DestroyTexture(TextureHandle handle)
{
    TextureObject texture = textures[handle.Id];

    MemoryBin::DestroyPersistentTexture(texture);

    ResourceRegistry::FreePersistentTextureSlot(handle);
}

void MemoryRegistry::DestroySampler(SamplerHandle handle)
{
    SamplerObject sampler = samplers[handle.Id];

    MemoryBin::DestroyPersistentSampler(sampler);

    ResourceRegistry::FreeSamplerSlot(handle);
}

void MemoryRegistry::DestroyBuffer(uint32_t id)
{
    BufferObject buffer = buffers[id];

    MemoryBin::DestroyPersistentBuffer(buffer);

    ResourceRegistry::FreePersistentBufferSlot(id);
}

void MemoryRegistry::DestroyTexture(uint32_t id)
{
    TextureObject texture = textures[id];

    MemoryBin::DestroyPersistentTexture(texture);

    ResourceRegistry::FreePersistentTextureSlot(id);
}

void MemoryRegistry::DestroySampler(uint32_t id)
{
    SamplerObject sampler = samplers[id];

    MemoryBin::DestroyPersistentSampler(sampler);

    ResourceRegistry::FreeSamplerSlot(id);
}

void MemoryRegistry::ResizeBufferIfNeeded(BufferHandle &buffer, uint64_t requiredSize)
{
    uint64_t currentSize = buffers[buffer.Id].AllocationInfo.size;

    if(currentSize < requiredSize)
    {
        MemoryRegistry::DestroyBuffer(buffer);

        BufferHandle newBufferHandle = MemoryRegistry::CreateGPUBuffer(requiredSize);

        buffer = newBufferHandle;
    }
}