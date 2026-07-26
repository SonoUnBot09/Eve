#pragma once

#include <graphics/GraphicsCore.hpp>
#include "VulkanMapping.hpp"
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

#include <vector>

namespace Eve::Graphics
{
    struct Texture
    {
        VkImage Image;
        VkImageView ImageView;
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
    };

    struct Sampler
    {
        VkSampler Sampler;
    };

    struct Buffer
    {
        VkBuffer Buffer;
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
    };

    class MemoryManager
    {
        public:
        
            static TextureHandle AllocateTexture1D(TextureInfo1D textureInfo);
            static TextureHandle AllocateTexture2D(TextureInfo2D textureInfo);
            static TextureHandle AllocateTexture3D(TextureInfo3D textureInfo);
            static SamplerHandle AllocateSampler(SamplerInfo samplerInfo);
            static BufferHandle AllocateBuffer(BufferInfo bufferInfo);

            static BufferHandle AllocateHostBuffer(BufferInfo bufferInfo);

            static VmaPool AllocateMemoryPool(uint32_t size);

            static TextureHandle ReserveTextureSlot(Texture& texture);
            static SamplerHandle ReserveSamplerSlot(Sampler sampler);
            static BufferHandle ReserveBufferSlot(Buffer& buffer); 
            static TransientTextureHandle ReserveTransientTextureSlot();
            static TransientBufferHandle ReserveTransientBufferSlot(); 

            inline static Texture& GetTexture(TextureHandle handle) { return textures[handle.Id]; }
            inline static Sampler GetSampler(SamplerHandle handle) { return samplers[handle.Id]; }
            inline static Buffer& GetBuffer(BufferHandle handle) { return buffers[handle.Id]; }
        private:

            inline static std::vector<Texture> textures;
            inline static std::vector<Sampler> samplers;
            inline static std::vector<Buffer> buffers;

            inline static std::vector<uint32_t> imageFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;
    };
}