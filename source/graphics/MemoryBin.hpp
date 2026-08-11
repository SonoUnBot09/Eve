#pragma once

#include <graphics/registers/MemoryRegistry.hpp>
#include <graphics/registers/TransientResourcePool.hpp>
#include "Resources.hpp"

namespace Eve::Graphics
{
    class MemoryBin
    {
        public:
            static void DestroyPendingResources();
            static void DestroyAllPendingResources(); 
            static void DestroyEverything();

            static void DestroyPersistentBuffer(BufferObject& buffer);
            static void DestroyPersistentTexture(TextureObject& texture);
            static void DestroyPersistentSampler(SamplerObject& sampler);
            static void DestroyTransientBuffer(TransientBufferObject& buffer, uint32_t countdown);
            static void DestroyTransientTexture(TransientTextureObject& texture, uint32_t countdown);
            static void DestroyMemoryBucket(MemoryBucket& memoryBucket);

        private:

            inline static std::vector<std::pair<BufferObject, uint32_t>> persistentBuffersToDestroy;
            inline static std::vector<std::pair<TextureObject, uint32_t>> persistentTexturesToDestroy;
            inline static std::vector<std::pair<SamplerObject, uint32_t>> persistentSamplersToDestroy;

            inline static std::vector<std::pair<TransientBufferObject, uint32_t>> transientBuffersToDestroy;
            inline static std::vector<std::pair<TransientTextureObject, uint32_t>> transientTexturesToDestroy;

            inline static std::vector<std::pair<MemoryBucket, uint32_t>> memoryBucketsToDestroy;



    };
}