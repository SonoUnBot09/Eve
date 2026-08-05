#include "MemoryRegistry.hpp"
#include "Eve/graphics/Buffer.hpp"
#include "Eve/graphics/PassModule.hpp"
#include "Eve/graphics/Texture.hpp"
#include "ResourceRegistry.hpp"
#include <graphics/ResourceMapper.hpp>
#include <graphics/GraphicsCore.hpp>
#include <graphics/MemoryBin.hpp>
#include <graphics/Resources.hpp>
#include <graphics/helpers/AllocationHelper.hpp>
#include <graphics/helpers/VulkanMapping.hpp>
#include <graphics/RenderGraph.hpp>
#include "ResourceRegistry.hpp"
#include "ResourceTracker.hpp"

using namespace Eve::Graphics;

TextureHandle MemoryRegistry::CreateTexture1D(TextureInfo1D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture1D(textureInfo, texture);

    TextureInfo info
    {
        .Data.TextureType = TextureType::TEXTURE_1D,
        .Data.Width = textureInfo.Width,
        .Data.Height = 1,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, info);

    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture2D(TextureInfo2D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture2D(textureInfo, texture);

    TextureInfo info
    {
        .Data.TextureType = TextureType::TEXTURE_2D,
        .Data.Width = textureInfo.Width,
        .Data.Height = textureInfo.Height,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, info);

    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture3D(TextureInfo3D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture3D(textureInfo, texture);

    TextureInfo info
    {
        .Data.TextureType = TextureType::TEXTURE_3D,
        .Data.Width = textureInfo.Width,
        .Data.Height = textureInfo.Height,
        .Data.Depth = textureInfo.Depth,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, info);


    textures.resize(ResourceRegistry::textureResourcesPeakIndex);
    texturesInfo.resize(ResourceRegistry::textureResourcesPeakIndex);

    textures[handle.Id] = texture;
    texturesInfo[handle.Id] = info;

    return handle;
}

TextureHandle MemoryRegistry::CreateTextureCube(TextureInfo2D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTextureCube(textureInfo, texture);

    TextureInfo info
    {
        .Data.TextureType = TextureType::TEXTURE_CUBE,
        .Data.Width = textureInfo.Width,
        .Data.Height = textureInfo.Height,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ResourceRegistry::RequestPersistentTextureSlot();

    ResourceMapper::ScheduleImageMapping(handle, info);


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

    ResourceMapper::ScheduleSamplerMapping(handle);

    samplers.resize(ResourceRegistry::samplerResourcesPeakIndex);
    samplersInfo.resize(ResourceRegistry::samplerResourcesPeakIndex);

    samplers[handle.Id] = sampler;
    samplersInfo[handle.Id] = info;

    return handle;
}

BufferHandle MemoryRegistry::CreateGPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateGPUBuffer(bufferInfo, buffer);

    BufferInfo info
    {
        .Data.Size = bufferInfo.Data.Size,
        .Data.Usage = bufferInfo.Data.Usage
    };

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();
    
    ResourceTracker::RegisterBufferState(handle);

    ResourceMapper::ScheduleBufferMapping(handle);

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
        .Data.Size = bufferInfo.Data.Size,
        .Data.Usage = bufferInfo.Data.Usage
    };

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();
    
    ResourceTracker::RegisterBufferState(handle);

    buffers.resize(ResourceRegistry::bufferResourcesPeakIndex);
    buffersInfo.resize(ResourceRegistry::bufferResourcesPeakIndex);

    buffers[handle.Id] = buffer;
    buffersInfo[handle.Id] = info;

    return handle;
}

void MemoryRegistry::DestroyBuffer(BufferHandle handle)
{
    BufferObject buffer = buffers[handle.Id];

    MemoryBin::DestroyBuffer(buffer);

    ResourceRegistry::FreeBufferSlot(handle);
}

void MemoryRegistry::DestroyTexture(TextureHandle handle)
{
    TextureObject texture = textures[handle.Id];

    MemoryBin::DestroyTexture(texture);

    ResourceRegistry::FreeTextureSlot(handle);
}

void MemoryRegistry::DestroySampler(SamplerHandle handle)
{
    SamplerObject sampler = samplers[handle.Id];

    MemoryBin::DestroySampler(sampler);

    ResourceRegistry::FreeSamplerSlot(handle);
}

void MemoryRegistry::DestroyBuffer(uint32_t id)
{
    BufferObject buffer = buffers[id];

    MemoryBin::DestroyBuffer(buffer);

    ResourceRegistry::FreeBufferSlot(id);
}

void MemoryRegistry::DestroyTexture(uint32_t id)
{
    TextureObject texture = textures[id];

    MemoryBin::DestroyTexture(texture);

    ResourceRegistry::FreeTextureSlot(id);
}

void MemoryRegistry::DestroySampler(uint32_t id)
{
    SamplerObject sampler = samplers[id];

    MemoryBin::DestroySampler(sampler);

    ResourceRegistry::FreeSamplerSlot(id);
}

void MemoryRegistry::ResizeBufferIfNeeded(BufferHandle &buffer, uint64_t requiredSize, bool indexBuffer)
{
    uint64_t currentSize = buffers[buffer.Id].AllocationInfo.size;

    if(currentSize < requiredSize)
    {
        MemoryRegistry::DestroyBuffer(buffer);

        BufferUsage bufferUsage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST;

        if(indexBuffer)
        {
            bufferUsage |= BufferUsage::BUFFER_USAGE_INDEX;
        }

        BufferInfo bufferInfo
        {
            .Data.Size = requiredSize,
            .Data.Usage = bufferUsage
        };

        BufferHandle newBufferHandle = MemoryRegistry::CreateGPUBuffer(bufferInfo);

        buffer = newBufferHandle;
    }
}