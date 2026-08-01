#pragma once

#include <graphics/GraphicsCore.hpp>
#include <graphics/helpers/AllocationHelper.hpp>


#include <vector>

namespace Eve::Graphics
{

    class MemoryRegistry
    {
        public:
        
            static TextureHandle CreateTexture1D(TextureInfo1D textureInfo);
            static TextureHandle CreateTexture2D(TextureInfo2D textureInfo);
            static TextureHandle CreateTexture3D(TextureInfo3D textureInfo);
            static SamplerHandle CreateSampler(SamplerInfo samplerInfo);
            static BufferHandle CreateGPUBuffer(BufferInfo bufferInfo);

            static BufferHandle CreateCPUBuffer(BufferInfo bufferInfo);

            static VmaPool AllocateMemoryPool(uint32_t size);

            static TextureHandle ReserveTextureSlot(TextureObject& texture);
            static SamplerHandle ReserveSamplerSlot(SamplerObject sampler);
            static BufferHandle ReserveGPUBufferSlot(BufferObject& buffer); 
            static BufferHandle ReserveCPUBufferSlot(BufferObject& buffer);
            static TransientTextureHandle ReserveTransientTextureSlot();
            static TransientBufferHandle ReserveTransientBufferSlot(); 

            static void DestroyBuffer(BufferHandle handle);
            static void DestroyTexture(TextureHandle handle);

            static void FreeTextureSlot(TextureHandle handle) { imageFreeSlots.push_back(handle.Id); }
            static void FreeTextureSlot(TransientTextureHandle handle) { imageFreeSlots.push_back(handle.Id); }
            static void FreeBufferSlot(BufferHandle handle) { bufferFreeSlots.push_back(handle.Id); }
            static void FreeBufferSlot(TransientBufferHandle handle) { bufferFreeSlots.push_back(handle.Id); }

            inline static TextureObject& GetTexture(TextureHandle handle) { return textures[handle.Id]; }
            inline static SamplerObject GetSampler(SamplerHandle handle) { return samplers[handle.Id]; }
            inline static BufferObject& GetBuffer(BufferHandle handle) { return buffers[handle.Id]; }
        private:

            inline static std::vector<TextureObject> textures;      inline static std::vector<uint32_t> textureGenerations;
            inline static std::vector<SamplerObject> samplers;      inline static std::vector<uint32_t> samplerGenerations;
            inline static std::vector<BufferObject> buffers;        inline static std::vector<uint32_t> bufferGenerations;

            inline static std::vector<uint32_t> imageFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;

            friend class MemoryBin;
    };
}