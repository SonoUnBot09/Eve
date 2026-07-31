#pragma once

#include "MemoryManager.hpp"

namespace Eve::Graphics
{
    class MemoryBin
    {
        public:
            static void DestroyPendingResources();
            static void DestroyBuffer(Buffer buffer);
            static void DestroyTexture(Texture texture);

            static void DestroyEverythingNow();
        private:
            inline static std::vector<std::pair<Buffer, uint32_t>> buffersToDestroy;
            inline static std::vector<std::pair<Texture, uint32_t>> texturesToDestroy;
    };
}