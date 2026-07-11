#pragma once

#include <graphics/api/vk/ContextBuilder.hpp>
#include <graphics/api/vk/VulkanMapping.hpp>
#include <Eve/graphics/Image.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

#include <vector>

namespace Eve::Graphics
{
    struct Image
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
            static ImageHandle AllocateImage(ImageInfo imageInfo);
            static SamplerHandle AllocateSampler(SamplerInfo samplerInfo);
            static BufferHandle AllocateBuffer(BufferInfo bufferInfo);
        private:

            inline static std::vector<Image> images;
            inline static std::vector<Sampler> samplers;
            inline static std::vector<Buffer> buffers;

            inline static std::vector<uint32_t> imageFreeSlots;
            inline static std::vector<uint32_t> samplerFreeSlots;
            inline static std::vector<uint32_t> bufferFreeSlots;
    };
}