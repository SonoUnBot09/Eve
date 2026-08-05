#pragma once

#include "MemoryRegistry.hpp"
#include <cstdint>
#include <vector>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    class ResourceRegistry
    {
        public:
            static TextureHandle RequestPersistentTextureSlot();
            static SamplerHandle RequestPersistentSamplerSlot();
            static BufferHandle RequestPersistentBufferSlot();
            static TransientTextureHandle RequestTransientTextureSlot();
            static TransientBufferHandle RequestTransientBufferSlot();

            static void FreeTextureSlot(TextureHandle handle);
            static void FreeTextureSlot(TransientTextureHandle handle);
            static void FreeTextureSlot(uint32_t id);
            static void FreeSamplerSlot(SamplerHandle handle);
            static void FreeSamplerSlot(uint32_t id);
            static void FreeBufferSlot(BufferHandle handle);
            static void FreeBufferSlot(TransientBufferHandle handle);
            static void FreeBufferSlot(uint32_t id);

        private:

            inline static std::vector<bool> persistentTextures;
            inline static std::vector<bool> persistentBuffers;
            inline static uint32_t textureResourcesPeakIndex = 0;
            inline static uint32_t samplerResourcesPeakIndex = 0;
            inline static uint32_t bufferResourcesPeakIndex = 0;

            // --- Generation Indicies ---
            inline static std::vector<uint32_t> textureGenerations;
            inline static std::vector<uint32_t> samplerGenerations;
            inline static std::vector<uint32_t> bufferGenerations;

            // --- Free Slots ---
            inline static std::vector<uint32_t> textureFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;

            friend class MemoryRegistry;
            friend class MemoryBin;
            friend class RenderGraph;
    };
}