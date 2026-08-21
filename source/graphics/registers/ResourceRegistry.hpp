#pragma once

#include "MemoryRegistry.hpp"
#include <cstdint>
#include <vector>
#include <eve/graphics/Texture.hpp>
#include <eve/graphics/Sampler.hpp>
#include <eve/graphics/Buffer.hpp>

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

            static void FreePersistentTextureSlot(TextureHandle handle);
            static void FreeTransientTextureSlot(TransientTextureHandle handle);
            static void FreePersistentTextureSlot(uint32_t id);
            static void FreeTransientTextureSlot(uint32_t id);

            static void FreeSamplerSlot(SamplerHandle handle);
            static void FreeSamplerSlot(uint32_t id);

            static void FreePersistentBufferSlot(BufferHandle handle);
            static void FreeTransientBufferSlot(TransientBufferHandle handle);
            static void FreePersistentBufferSlot(uint32_t id);
            static void FreeTransientBufferSlot(uint32_t id);

        private:

            inline static std::vector<bool> persistentTextures;
            inline static std::vector<bool> persistentBuffers;
            inline static std::vector<bool> persistentSamplers;
            inline static std::vector<bool> transientTextures;
            inline static std::vector<bool> transientBuffers;

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