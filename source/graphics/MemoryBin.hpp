#pragma once

#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"

namespace Eve::Graphics
{
    class MemoryBin
    {
        public:
            static void DestroyPendingResources();
            static void DestroyBuffer(BufferObject buffer);
            static void DestroyTexture(TextureObject texture);
            static void DestroySampler(SamplerObject sampler);

            static void DestroyEverythingNow();

            inline static void MarkGPUAsIdle() { isGPUInIdle = true; }

        private:
            inline static std::vector<std::pair<BufferObject, uint32_t>> buffersToDestroy;
            inline static std::vector<std::pair<TextureObject, uint32_t>> texturesToDestroy;
            inline static std::vector<std::pair<SamplerObject, uint32_t>> samplersToDestroy;

            inline static bool isGPUInIdle = false;


    };
}