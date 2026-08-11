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
        transientTextures.push_back(false);
    }
    else
    {
        uint32_t id = textureFreeSlots.back();
        textureFreeSlots.pop_back();

        textureGenerations[id]++;

        handle.Id = id;
        handle.Generation = textureGenerations[id];

        persistentTextures[id] = true;
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

        persistentSamplers.push_back(true);
    }
    else
    {
        uint32_t id = samplerFreeSlots.back();
        samplerFreeSlots.pop_back();

        samplerGenerations[id]++;

        handle.Id = id;
        handle.Generation = samplerGenerations[id];

        persistentSamplers[handle.Id] = true;
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
        transientBuffers.push_back(false);
    }
    else
    {
        uint32_t id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        bufferGenerations[id]++;

        handle.Id = id;
        handle.Generation = bufferGenerations[id];

        persistentBuffers[id] = true;
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
        transientTextures.push_back(true);
    }
    else
    {
        uint32_t id = textureFreeSlots.back();
        textureFreeSlots.pop_back();

        handle.Id = id;

        transientTextures[handle.Id] = true;
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
        transientBuffers.push_back(true);
    }
    else
    {
        uint32_t id = bufferFreeSlots.back();
        bufferFreeSlots.pop_back();

        handle.Id = id;

        transientBuffers[handle.Id] = true;
    }

    ResourceTracker::RegisterBufferState(handle);

    return handle;
}

void ResourceRegistry::FreePersistentTextureSlot(TextureHandle handle)
{
    textureFreeSlots.push_back(handle.Id);
    persistentTextures[handle.Id] = false;
}
void ResourceRegistry::FreeTransientTextureSlot(TransientTextureHandle handle)
{
    textureFreeSlots.push_back(handle.Id);
    transientTextures[handle.Id] = false;
}
void ResourceRegistry::FreePersistentTextureSlot(uint32_t id)
{
    textureFreeSlots.push_back(id);
    persistentTextures[id] = false;
}
void ResourceRegistry::FreeTransientTextureSlot(uint32_t id)
{
    textureFreeSlots.push_back(id);
    transientTextures[id] = false;
}

void ResourceRegistry::FreeSamplerSlot(SamplerHandle handle)
{
    samplerFreeSlots.push_back(handle.Id);
    persistentSamplers[handle.Id] = false;
}
void ResourceRegistry::FreeSamplerSlot(uint32_t id)
{
    samplerFreeSlots.push_back(id);
    persistentSamplers[id] = false;
}

void ResourceRegistry::FreePersistentBufferSlot(BufferHandle handle)
{
    bufferFreeSlots.push_back(handle.Id);
    persistentBuffers[handle.Id] = false;
}
void ResourceRegistry::FreeTransientBufferSlot(TransientBufferHandle handle)
{
    bufferFreeSlots.push_back(handle.Id);
    transientBuffers[handle.Id] = false;
}
void ResourceRegistry::FreePersistentBufferSlot(uint32_t id)
{
    bufferFreeSlots.push_back(id);
    persistentBuffers[id] = false;
}
void ResourceRegistry::FreeTransientBufferSlot(uint32_t id)
{
    bufferFreeSlots.push_back(id);
    transientBuffers[id] = false;
}