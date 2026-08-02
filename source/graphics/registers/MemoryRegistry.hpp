#pragma once

#include "Eve/graphics/Buffer.hpp"
#include "Eve/graphics/Sampler.hpp"
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

            // --- Sampler Creation ---
            static SamplerHandle CreateSampler(SamplerInfo samplerInfo);

            // --- Buffer Creation ---
            static BufferHandle CreateGPUBuffer(BufferInfo bufferInfo);
            static BufferHandle CreateCPUBuffer(BufferInfo bufferInfo);

            static TextureHandle ReserveTextureSlot(TextureObject& texture, TextureInfo& textureInfo);
            static SamplerHandle ReserveSamplerSlot(SamplerObject sampler, SamplerInfo& samplerInfo);
            static BufferHandle ReserveGPUBufferSlot(BufferObject& buffer, BufferInfo& bufferInfo); 
            static BufferHandle ReserveCPUBufferSlot(BufferObject& buffer);

            static TransientTextureHandle ReserveTransientTextureSlot();
            static TransientBufferHandle ReserveTransientBufferSlot(); 

            // --- Resource Destruction ---
            static void DestroyBuffer(BufferHandle handle);
            static void DestroyTexture(TextureHandle handle);
            static void DestroySampler(SamplerHandle handle);
            static void DestroyBuffer(uint32_t id);
            static void DestroyTexture(uint32_t id);
            static void DestroySampler(uint32_t id);

            static void FreeTextureSlot(TextureHandle handle) { imageFreeSlots.push_back(handle.Id); }
            static void FreeTextureSlot(TransientTextureHandle handle) { imageFreeSlots.push_back(handle.Id); }
            static void FreeTextureSlot(uint32_t id) { imageFreeSlots.push_back(id); }
            static void FreeBufferSlot(BufferHandle handle) { bufferFreeSlots.push_back(handle.Id); }
            static void FreeBufferSlot(TransientBufferHandle handle) { bufferFreeSlots.push_back(handle.Id); }
            static void FreeBufferSlot(uint32_t id) { bufferFreeSlots.push_back(id); }
            static void FreeSamplerSlot(SamplerHandle handle) { samplerFreeSlots.push_back(handle.Id); }
            static void FreeSamplerSlot(uint32_t id) { samplerFreeSlots.push_back(id); }

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
            
        private:

            inline static std::vector<TextureObject> textures;      inline static std::vector<uint32_t> textureGenerations;
            inline static std::vector<SamplerObject> samplers;      inline static std::vector<uint32_t> samplerGenerations;
            inline static std::vector<BufferObject> buffers;        inline static std::vector<uint32_t> bufferGenerations;

            inline static std::vector<TextureInfo> texturesInfo;
            inline static std::vector<SamplerInfo> samplersInfo;
            inline static std::vector<BufferInfo> buffersInfo;

            inline static std::vector<uint32_t> imageFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;

            friend class MemoryBin;
    };
}