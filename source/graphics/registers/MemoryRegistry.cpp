#include "MemoryRegistry.hpp"
#include <graphics/ResourceMapper.hpp>
#include <graphics/GraphicsCore.hpp>
#include <graphics/MemoryBin.hpp>
#include <graphics/Resources.hpp>
#include <graphics/helpers/AllocationHelper.hpp>
#include <graphics/helpers/VulkanMapping.hpp>

using namespace Eve::Graphics;

TextureHandle MemoryRegistry::CreateTexture1D(TextureInfo1D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture1D(textureInfo, texture);

    TextureHandle handle = ReserveTextureSlot(texture);

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture2D(TextureInfo2D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture2D(textureInfo, texture);

    TextureHandle handle = ReserveTextureSlot(texture);

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture3D(TextureInfo3D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture3D(textureInfo, texture);

    TextureHandle handle = ReserveTextureSlot(texture);

    return handle;
}

SamplerHandle MemoryRegistry::CreateSampler(SamplerInfo samplerInfo)
{
    SamplerObject sampler {};
    Helpers::AllocateSampler(samplerInfo, sampler);

    SamplerHandle handle = ReserveSamplerSlot(sampler);

    return handle;
}

BufferHandle MemoryRegistry::CreateGPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateGPUBuffer(bufferInfo, buffer);

    BufferHandle handle = ReserveGPUBufferSlot(buffer);

    return handle;
}

BufferHandle MemoryRegistry::CreateCPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateCPUBuffer(bufferInfo, buffer);

    BufferHandle handle = ReserveGPUBufferSlot(buffer);

    return handle;
}

VmaPool MemoryRegistry::AllocateMemoryPool(uint32_t size)
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

void MemoryRegistry::DestroyBuffer(BufferHandle handle)
{
    BufferObject buffer = buffers[handle.Id];

    MemoryBin::DestroyBuffer(buffer);

    FreeBufferSlot(handle);
}

void MemoryRegistry::DestroyTexture(TextureHandle handle)
{
    TextureObject texture = textures[handle.Id];

    MemoryBin::DestroyTexture(texture);

    FreeTextureSlot(handle);
}

TextureHandle MemoryRegistry::ReserveTextureSlot(TextureObject& texture)
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

SamplerHandle MemoryRegistry::ReserveSamplerSlot(SamplerObject sampler)
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

BufferHandle MemoryRegistry::ReserveGPUBufferSlot(BufferObject& buffer)
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

BufferHandle MemoryRegistry::ReserveCPUBufferSlot(BufferObject& buffer)
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

    return handle;
}

TransientTextureHandle MemoryRegistry::ReserveTransientTextureSlot()
{
    TextureHandle handle;
    TextureObject texture {0, 0, 0, 0};
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

TransientBufferHandle MemoryRegistry::ReserveTransientBufferSlot()
{
    BufferHandle handle;
    BufferObject buffer{0,0,0};
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