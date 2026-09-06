#include "ResourceRegistry.hpp"
#include "EveSettings.hpp"
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

void ResourceRegistry::DestroyPendingResources()
{
    for(int32_t i = static_cast<int32_t>(transientTexturePendingDestruction.size()) - 1; i >= 0; i--)
    {
        std::pair<uint32_t, uint32_t>& pair = transientTexturePendingDestruction[i];

        if(pair.second == 0)
        {
            textureFreeSlots.push_back(pair.first);
            transientTexturePendingDestruction.erase(transientTexturePendingDestruction.begin() + i);
        }
        else 
        {
            pair.second--;
        }
    }

    for(int32_t i = static_cast<int32_t>(persistentTexturePendingDestruction.size()) - 1; i >= 0; i--)
    {
        std::pair<uint32_t, uint32_t>& pair = persistentTexturePendingDestruction[i];

        if(pair.second == 0)
        {
            textureFreeSlots.push_back(pair.first);
            persistentTexturePendingDestruction.erase(persistentTexturePendingDestruction.begin() + i);
        }
        else 
        {
            pair.second--;
        }
    }

    for(int32_t i = static_cast<int32_t>(persistentSamplerPendingDestruction.size()) - 1; i >= 0; i--)
    {
        std::pair<uint32_t, uint32_t>& pair = persistentSamplerPendingDestruction[i];

        if(pair.second == 0)
        {
            samplerFreeSlots.push_back(pair.first);
            persistentSamplerPendingDestruction.erase(persistentSamplerPendingDestruction.begin() + i);
        }
        else 
        {
            pair.second--;
        }
    }

    for(int32_t i = static_cast<int32_t>(transientBufferPendingDestruction.size()) - 1; i >= 0; i--)
    {
        std::pair<uint32_t, uint32_t>& pair = transientBufferPendingDestruction[i];

        if(pair.second == 0)
        {
            bufferFreeSlots.push_back(pair.first);
            transientBufferPendingDestruction.erase(transientBufferPendingDestruction.begin() + i);
        }
        else 
        {
            pair.second--;
        }
    }

    for(int32_t i = static_cast<int32_t>(persistentBufferPendingDestruction.size()) - 1; i >= 0; i--)
    {
        std::pair<uint32_t, uint32_t>& pair = persistentBufferPendingDestruction[i];

        if(pair.second == 0)
        {
            bufferFreeSlots.push_back(pair.first);
            persistentBufferPendingDestruction.erase(persistentBufferPendingDestruction.begin() + i);
        }
        else 
        {
            pair.second--;
        }
    }
}

void ResourceRegistry::FreePersistentTextureSlot(TextureHandle handle)
{
    persistentTextures[handle.Id] = false;
    persistentTexturePendingDestruction.emplace_back(handle.Id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreeTransientTextureSlot(TransientTextureHandle handle)
{
    transientTextures[handle.Id] = false;
    transientTexturePendingDestruction.emplace_back(handle.Id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreePersistentTextureSlot(uint32_t id)
{
    persistentTextures[id] = false;
    persistentTexturePendingDestruction.emplace_back(id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreeTransientTextureSlot(uint32_t id)
{
    transientTextures[id] = false;
    transientTexturePendingDestruction.emplace_back(id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}

void ResourceRegistry::FreeSamplerSlot(SamplerHandle handle)
{
    persistentSamplers[handle.Id] = false;
    persistentSamplerPendingDestruction.emplace_back(handle.Id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreeSamplerSlot(uint32_t id)
{
    persistentSamplers[id] = false;
    persistentSamplerPendingDestruction.emplace_back(id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}

void ResourceRegistry::FreePersistentBufferSlot(BufferHandle handle)
{
    persistentBuffers[handle.Id] = false;
    persistentBufferPendingDestruction.emplace_back(handle.Id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreeTransientBufferSlot(TransientBufferHandle handle)
{
    transientBuffers[handle.Id] = false;
    transientBufferPendingDestruction.emplace_back(handle.Id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreePersistentBufferSlot(uint32_t id)
{
    persistentBuffers[id] = false;
    persistentBufferPendingDestruction.emplace_back(id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}
void ResourceRegistry::FreeTransientBufferSlot(uint32_t id)
{
    transientBuffers[id] = false;
    transientBufferPendingDestruction.emplace_back(id, Eve::Settings::MAX_FRAMES_IN_FLIGHT);
}