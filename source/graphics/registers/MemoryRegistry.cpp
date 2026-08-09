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
        .TextureType = TextureType::TEXTURE_1D,
        .Width = textureInfo.Width,
        .Height = 1,
        .Depth = 1,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Usage = textureInfo.Usage,
        .Sample = textureInfo.Sample
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
    TextureObject texture {};
    Helpers::AllocateTexture2D(textureInfo, texture);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_2D,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Usage = textureInfo.Usage,
        .Sample = textureInfo.Sample
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
    TextureObject texture {};
    Helpers::AllocateTexture3D(textureInfo, texture);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_3D,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = textureInfo.Depth,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Usage = textureInfo.Usage,
        .Sample = textureInfo.Sample,
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
    TextureObject texture {};
    Helpers::AllocateTextureCube(textureInfo, texture);

    TextureInfo info
    {
        .TextureType = TextureType::TEXTURE_CUBE,
        .Width = textureInfo.Width,
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Usage = textureInfo.Usage,
        .Sample = textureInfo.Sample
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

BufferHandle MemoryRegistry::CreateGPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateGPUBuffer(bufferInfo, buffer);

    BufferInfo info
    {
        .Size = bufferInfo.Size,
        .Usage = bufferInfo.Usage
    };

    BufferHandle handle = ResourceRegistry::RequestPersistentBufferSlot();
    
    ResourceTracker::RegisterBufferState(handle);

    // TODO : Mapping ResourceMapper::ScheduleBufferMapping(handle);

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
            .Size = requiredSize,
            .Usage = bufferUsage
        };

        BufferHandle newBufferHandle = MemoryRegistry::CreateGPUBuffer(bufferInfo);

        buffer = newBufferHandle;
    }
}