#pragma once

#include "Eve/graphics/Buffer.hpp"
#include "Eve/graphics/Sampler.hpp"
#include "Eve/graphics/Texture.hpp"
#include <graphics/GraphicsCore.hpp>
#include <graphics/helpers/AllocationHelper.hpp>


#include <vector>

namespace Eve::Graphics
{
    class MemoryRegistry
    {
        public:
        
            // --- Texture Creation ---
            static TextureHandle CreateTexture1D(TextureInfo1D textureInfo);
            static TextureHandle CreateTexture2D(TextureInfo2D textureInfo);
            static TextureHandle CreateTexture3D(TextureInfo3D textureInfo);
            static TextureHandle CreateTextureCube(TextureInfo2D textureInfo);

            // --- Sampler Creation ---
            static SamplerHandle CreateSampler(SamplerInfo samplerInfo);

            // --- Buffer Creation ---
            static BufferHandle CreateGPUBuffer(BufferInfo bufferInfo);
            static BufferHandle CreateCPUBuffer(BufferInfo bufferInfo);


            // --- Resource Destruction ---
            static void DestroyBuffer(BufferHandle handle);
            static void DestroyTexture(TextureHandle handle);
            static void DestroySampler(SamplerHandle handle);
            static void DestroyBuffer(uint32_t id);
            static void DestroyTexture(uint32_t id);
            static void DestroySampler(uint32_t id);

            // --- Getters ---
            inline static TextureObject& GetTexture(TextureHandle handle) { return textures[handle.Id]; }
            inline static SamplerObject GetSampler(SamplerHandle handle) { return samplers[handle.Id]; }
            inline static BufferObject& GetBuffer(BufferHandle handle) { return buffers[handle.Id]; }
            inline static TextureObject& GetTexture(uint32_t id) { return textures[id]; }
            inline static SamplerObject GetSampler(uint32_t id) { return samplers[id]; }
            inline static BufferObject& GetBuffer(uint32_t id) { return buffers[id]; }

            inline static TextureInfo& GetTextureInfo(TextureHandle handle) { return texturesInfo[handle.Id]; }
            inline static TextureInfo& GetTextureInfo(uint32_t id) { return texturesInfo[id]; }
            inline static SamplerInfo& GetSamplerInfo(TextureHandle handle) { return samplersInfo[handle.Id]; }
            inline static SamplerInfo& GetSamplerInfo(uint32_t id) { return samplersInfo[id]; }
            inline static BufferInfo& GetBufferInfo(BufferHandle handle) { return buffersInfo[handle.Id]; }
            inline static BufferInfo& GetBufferInfo(uint32_t id) { return buffersInfo[id]; }

            // --- Helpers ---
            static void ResizeBufferIfNeeded(BufferHandle& buffer, uint64_t requiredSize, bool indexBuffer);
            
        private:

            inline static std::vector<TextureObject> textures;
            inline static std::vector<SamplerObject> samplers;
            inline static std::vector<BufferObject> buffers;

            inline static std::vector<TextureInfo> texturesInfo;
            inline static std::vector<SamplerInfo> samplersInfo;
            inline static std::vector<BufferInfo> buffersInfo;

            friend class MemoryBin;
            friend class RenderGraph;
    };
}