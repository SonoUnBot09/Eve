#include "MemoryBin.hpp"
#include "EveSettings.hpp"
#include "GraphicsCore.hpp"
#include "MemoryManager.hpp"

using namespace Eve::Graphics;

void MemoryBin::DestroyPendingResources()
{
    for(uint32_t i = 0; i < buffersToDestroy.size(); i++)
    {
        if(buffersToDestroy[i].second > 0)
        {
            buffersToDestroy[i].second--;
            continue;
        }

        Buffer buffer = buffersToDestroy[i].first;

        vmaDestroyBuffer(GraphicsCore::Context.Allocator, buffer.Buffer, buffer.Allocation);

        buffersToDestroy.erase(buffersToDestroy.begin() + i);
    }

    for(uint32_t i = 0; i < texturesToDestroy.size(); i++)
    {
        if(texturesToDestroy[i].second > 0)
        {
            texturesToDestroy[i].second--;
            continue;
        }

        Texture texture = texturesToDestroy[i].first;

        vkDestroyImageView(GraphicsCore::Context.Device, texture.ImageView, nullptr);
        
        vmaDestroyImage(GraphicsCore::Context.Allocator, texture.Image, texture.Allocation);

        texturesToDestroy.erase(texturesToDestroy.begin() + i);
    }
}

void MemoryBin::DestroyBuffer(Buffer buffer)
{
    buffersToDestroy.push_back(std::pair{buffer, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}

void MemoryBin::DestroyTexture(Texture texture)
{
    texturesToDestroy.push_back(std::pair{texture, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
}