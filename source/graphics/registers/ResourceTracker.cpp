#include "ResourceTracker.hpp"

using namespace Eve::Graphics;

void ResourceTracker::RegisterTextureState(TextureHandle handle)
{
    uint32_t currentSize = textureStates.size();

    TextureState state
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .Usage = static_cast<Usage>(0)
    };

    if(handle.Id == currentSize)
    {
        textureStates.push_back(state);
    }
    else 
    {
        textureStates[handle.Id] = state;
    }
}

void ResourceTracker::RegisterTextureState(TransientTextureHandle handle)
{
    uint32_t currentSize = textureStates.size();

    TextureState state
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Layout = VK_IMAGE_LAYOUT_UNDEFINED,
        .Usage = static_cast<Usage>(0)
    };

    if(handle.Id == currentSize)
    {
        textureStates.push_back(state);
    }
    else 
    {
        textureStates[handle.Id] = state;
    }
}

void ResourceTracker::RegisterBufferState(BufferHandle handle)
{
    uint32_t currentSize = bufferStates.size();

    BufferState state
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Usage = static_cast<Usage>(0)
    };

    if(handle.Id == currentSize)
    {
        bufferStates.push_back(state);
    }
    else 
    {
        bufferStates[handle.Id] = state;
    }
}

void ResourceTracker::RegisterBufferState(TransientBufferHandle handle)
{
    uint32_t currentSize = bufferStates.size();

    BufferState state
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Usage = static_cast<Usage>(0)
    };

    if(handle.Id == currentSize)
    {
        bufferStates.push_back(state);
    }
    else 
    {
        bufferStates[handle.Id] = state;
    }
}