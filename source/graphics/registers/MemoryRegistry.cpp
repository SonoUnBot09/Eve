#include "MemoryRegistry.hpp"
#include "Eve/graphics/PassModule.hpp"
#include <graphics/ResourceMapper.hpp>
#include <graphics/GraphicsCore.hpp>
#include <graphics/MemoryBin.hpp>
#include <graphics/Resources.hpp>
#include <graphics/helpers/AllocationHelper.hpp>
#include <graphics/helpers/VulkanMapping.hpp>
#include <graphics/RenderGraph.hpp>

using namespace Eve::Graphics;

TextureHandle MemoryRegistry::CreateTexture1D(TextureInfo1D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture1D(textureInfo, texture);

    TextureInfo info
    {
        .Data.Width = textureInfo.Width,
        .Data.Height = 1,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ReserveTextureSlot(texture, info);

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture2D(TextureInfo2D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture2D(textureInfo, texture);

    TextureInfo info
    {
        .Data.Width = textureInfo.Width,
        .Data.Height = textureInfo.Height,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ReserveTextureSlot(texture, info);

    return handle;
}

TextureHandle MemoryRegistry::CreateTexture3D(TextureInfo3D textureInfo)
{
    TextureObject texture {};
    Helpers::AllocateTexture3D(textureInfo, texture);

    TextureInfo info
    {
        .Data.Width = textureInfo.Width,
        .Data.Height = textureInfo.Height,
        .Data.Depth = textureInfo.Depth,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.Format = textureInfo.Format,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Sample = textureInfo.Sample,
        .Data.Usage = textureInfo.Usage
    };

    TextureHandle handle = ReserveTextureSlot(texture, info);

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

    SamplerHandle handle = ReserveSamplerSlot(sampler, info);

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

    BufferHandle handle = ReserveGPUBufferSlot(buffer, info);

    return handle;
}

BufferHandle MemoryRegistry::CreateCPUBuffer(BufferInfo bufferInfo)
{
    BufferObject buffer {};
    Helpers::AllocateCPUBuffer(bufferInfo, buffer);

    BufferHandle handle = ReserveCPUBufferSlot(buffer);

    return handle;
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

void MemoryRegistry::DestroySampler(SamplerHandle handle)
{
    SamplerObject sampler = samplers[handle.Id];

    MemoryBin::DestroySampler(sampler);

    FreeSamplerSlot(handle);
}

void MemoryRegistry::DestroyBuffer(uint32_t id)
{
    BufferObject buffer = buffers[id];

    MemoryBin::DestroyBuffer(buffer);

    FreeBufferSlot(id);
}

void MemoryRegistry::DestroyTexture(uint32_t id)
{
    TextureObject texture = textures[id];

    MemoryBin::DestroyTexture(texture);

    FreeTextureSlot(id);
}

void MemoryRegistry::DestroySampler(uint32_t id)
{
    SamplerObject sampler = samplers[id];

    MemoryBin::DestroySampler(sampler);

    FreeSamplerSlot(id);
}

TextureHandle MemoryRegistry::ReserveTextureSlot(TextureObject& texture, TextureInfo& textureInfo)
{
    TextureHandle handle;
    if(imageFreeSlots.empty()) 
    {
        uint32_t size = textures.size(); 

        textureGenerations.resize(size + 1);

        handle.Id = size;
        handle.Generation = textureGenerations[handle.Id];

        textureGenerations[handle.Id]++;

        textures.push_back(texture);
        texturesInfo.push_back(textureInfo);

        // --- Update the Render Graph with the new resource state ---
        RenderGraph::persistentTexturesState.push_back(RenderGraph::PersistentTextureState
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Layout = VK_IMAGE_LAYOUT_UNDEFINED,
            .Usage = static_cast<Usage>(0)
        });
    }
    else 
    {
        handle.Id = imageFreeSlots.back();
        handle.Generation = textureGenerations[handle.Id];

        textureGenerations[handle.Id]++;

        imageFreeSlots.pop_back();

        textures[handle.Id] = texture;
        texturesInfo[handle.Id] = textureInfo;

        // --- Update the Render Graph with the new resource state ---
        RenderGraph::persistentTexturesState[handle.Id] = RenderGraph::PersistentTextureState
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Layout = VK_IMAGE_LAYOUT_UNDEFINED,
            .Usage = static_cast<Usage>(0)
        };
    }

    ResourceMapper::ScheduleImageMapping(handle);



    return handle;
}

SamplerHandle MemoryRegistry::ReserveSamplerSlot(SamplerObject sampler, SamplerInfo& samplerInfo)
{
    SamplerHandle handle;
    if(samplerFreeSlots.empty()) 
    {
        uint32_t size = samplers.size(); 

        samplerGenerations.resize(size + 1);

        handle.Id = size;
        handle.Generation = samplerGenerations[handle.Id];

        samplerGenerations[handle.Id]++;

        samplers.push_back(sampler);
        samplersInfo.push_back(samplerInfo);
    }
    else 
    {
        handle.Id = samplerFreeSlots.back();
        handle.Generation = samplerGenerations[handle.Id];

        samplerGenerations[handle.Id]++;

        samplerFreeSlots.pop_back();

        samplers[handle.Id] = sampler;
        samplersInfo[handle.Id] = samplerInfo;
    }

    ResourceMapper::ScheduleSamplerMapping(handle);

    return handle;
}

BufferHandle MemoryRegistry::ReserveGPUBufferSlot(BufferObject& buffer, BufferInfo& bufferInfo)
{
    BufferHandle handle;
    if(bufferFreeSlots.empty()) 
    {
        uint32_t size = buffers.size(); 

        bufferGenerations.resize(size + 1);

        handle.Id = size;
        handle.Generation = bufferGenerations[handle.Id];

        bufferGenerations[handle.Id]++;

        buffers.push_back(buffer);
        buffersInfo.push_back(bufferInfo);

        // --- Update the Render Graph with the new resource state ---
        RenderGraph::persistentBuffersState.push_back(RenderGraph::PersistentBufferState
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Usage = static_cast<Usage>(0)
        });
    }
    else 
    {
        handle.Id = bufferFreeSlots.back();
        handle.Generation = bufferGenerations[handle.Id];

        bufferGenerations[handle.Id]++;

        bufferFreeSlots.pop_back();

        buffers[handle.Id] = buffer;
        buffersInfo[handle.Id] = bufferInfo;

        // --- Update the Render Graph with the new resource state ---
        RenderGraph::persistentBuffersState[handle.Id] = RenderGraph::PersistentBufferState
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Usage = static_cast<Usage>(0)
        };
    }

    ResourceMapper::ScheduleBufferMapping(handle);

    return handle;
}

BufferHandle MemoryRegistry::ReserveCPUBufferSlot(BufferObject& buffer)
{
    BufferHandle handle;
    if(bufferFreeSlots.empty()) 
    {
        uint32_t size = buffers.size(); 

        bufferGenerations.resize(size + 1);

        handle.Id = size;
        handle.Generation = bufferGenerations[handle.Id];

        bufferGenerations[handle.Id]++;

        buffers.push_back(buffer);
        
        BufferInfo dummyInfo {};
        buffersInfo.push_back(dummyInfo);
    }
    else 
    {
        handle.Id = bufferFreeSlots.back();
        handle.Generation = bufferGenerations[handle.Id];

        bufferGenerations[handle.Id]++;

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
        uint32_t size = textures.size(); 

        textureGenerations.resize(size + 1);

        handle.Id = size;

        textures.push_back(texture);

        TextureInfo dummyInfo{};
        texturesInfo.push_back(dummyInfo);
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
        uint32_t size = buffers.size(); 

        bufferGenerations.resize(size + 1);

        handle.Id = size;

        buffers.push_back(buffer);

        BufferInfo dummyInfo{};
        buffersInfo.push_back(dummyInfo);
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