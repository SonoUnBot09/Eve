#include "ResourceRegistry.hpp"
#include "ResourceTracker.hpp"

using namespace Eve::Graphics;

TextureHandle ResourceRegistry::RequestPersistentTextureSlot()
{
    TextureHandle handle;

    if(textureFreeSlots.empty())
    {
        handle.Id = textureResourcesPeakIndex;
        textureResourcesPeakIndex++;
        handle.Generation = 0;

        textureGenerations.push_back(0);
        persistentTextures.push_back(true);
    }
    else
    {
        uint32_t id = textureFreeSlots.back();
        textureFreeSlots.pop_back();

        textureGenerations[id]++;
        persistentTextures[id] = true;

        handle.Id = id;
        handle.Generation = textureGenerations[id];
        
    }

    ResourceTracker::RegisterTextureState(handle);

    return handle;
}

SamplerHandle ResourceRegistry::RequestPersistentSamplerSlot()
{
    SamplerHandle handle;

    if(samplerFreeSlots.empty())
    {
        handle.Id = samplerResourcesPeakIndex;
        samplerResourcesPeakIndex++;
        handle.Generation = 0;

        samplerGenerations.push_back(0);
    }
    else
    {
        uint32_t id = samplerFreeSlots.back();
        samplerFreeSlots.pop_back();

        samplerGenerations[id]++;

        handle.Id = id;
        handle.Generation = samplerGenerations[id];
    }

    return handle;
}

BufferHandle ResourceRegistry::RequestPersistentBufferSlot()
{
    BufferHandle handle;

    if(bufferFreeSlots.empty())
    {
        handle.Id = bufferResourcesPeakIndex;
        bufferResourcesPeakIndex++;
        handle.Generation = 0;

        bufferGenerations.push_back(0);
        persistentBuffers.push_back(true);
    }
    else
    {
        uint32_t id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        bufferGenerations[id]++;
        persistentBuffers[id] = true;

        handle.Id = id;
        handle.Generation = bufferGenerations[id];
    }

    ResourceTracker::RegisterBufferState(handle);

    return handle;
}

TransientTextureHandle ResourceRegistry::RequestTransientTextureSlot()
{
    TransientTextureHandle handle;

    if(textureFreeSlots.empty())
    {
        handle.Id = textureResourcesPeakIndex;
        textureResourcesPeakIndex++;

        textureGenerations.push_back(0);
        persistentTextures.push_back(false);
    }
    else
    {
        uint32_t id = textureFreeSlots.back();
        textureFreeSlots.pop_back();

        handle.Id = id;
    }

    ResourceTracker::RegisterTextureState(handle);

    return handle;
}

TransientBufferHandle ResourceRegistry::RequestTransientBufferSlot()
{
    TransientBufferHandle handle;

    if(bufferFreeSlots.empty())
    {
        handle.Id = bufferResourcesPeakIndex;
        bufferResourcesPeakIndex++;

        bufferGenerations.push_back(0);
        persistentBuffers.push_back(false);
    }
    else
    {
        uint32_t id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        handle.Id = id;
    }

    ResourceTracker::RegisterBufferState(handle);

    return handle;
}

void ResourceRegistry::FreeTextureSlot(TextureHandle handle)
{
    textureFreeSlots.push_back(handle.Id);
    persistentTextures[handle.Id] = false;
}
void ResourceRegistry::FreeTextureSlot(TransientTextureHandle handle)
{
    textureFreeSlots.push_back(handle.Id);
    persistentTextures[handle.Id] = false;
}
void ResourceRegistry::FreeTextureSlot(uint32_t id)
{
    textureFreeSlots.push_back(id);
    persistentTextures[id] = false;
}

void ResourceRegistry::FreeSamplerSlot(SamplerHandle handle)
{
    samplerFreeSlots.push_back(handle.Id);
}
void ResourceRegistry::FreeSamplerSlot(uint32_t id)
{
    samplerFreeSlots.push_back(id);
}

void ResourceRegistry::FreeBufferSlot(BufferHandle handle)
{
    bufferFreeSlots.push_back(handle.Id);
    persistentBuffers[handle.Id] = false;
}
void ResourceRegistry::FreeBufferSlot(TransientBufferHandle handle)
{
    bufferFreeSlots.push_back(handle.Id);
    persistentBuffers[handle.Id] = false;
}
void ResourceRegistry::FreeBufferSlot(uint32_t id)
{
    bufferFreeSlots.push_back(id);
    persistentBuffers[id] = false;
}