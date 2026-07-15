#pragma once

#include <graphics/api/vk/ContextBuilder.hpp>
#include <graphics/api/vk/VulkanMapping.hpp>
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
            static TextureHandle AllocateTexture(TextureInfo textureInfo);
            static SamplerHandle AllocateSampler(SamplerInfo samplerInfo);
            static BufferHandle AllocateBuffer(BufferInfo bufferInfo);

            static BufferHandle AllocateHostBuffer(BufferInfo bufferInfo);

            inline static Texture& GetTexture(TextureHandle handle) { return images[handle.Id]; }
            inline static Sampler GetSampler(SamplerHandle handle) { return samplers[handle.Id]; }
            inline static Buffer& GetBuffer(BufferHandle handle) { return buffers[handle.Id]; }
        private:

            inline static std::vector<Texture> images;
            inline static std::vector<Sampler> samplers;
            inline static std::vector<Buffer> buffers;

            inline static std::vector<uint32_t> imageFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;
    };
}