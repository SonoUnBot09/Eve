#pragma once

#include <vulkan/vulkan.hpp>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Buffer.hpp>
#include <EveSettings.hpp>
#include <graphics/Resources.hpp>

#include <vector>
#include <array>

namespace Eve::Graphics
{
    struct MemoryInfo
    {
        uint64_t Size;
        uint64_t Alignment;
        uint32_t BucketIndex;
    };

    struct TexturePool
    {
        TextureInfo TextureInfo;
        MemoryInfo MemoryInfo;
        uint32_t Countdown;
        
        std::vector<TransientTextureObject> Textures;
    };

    struct BufferPool
    {
        BufferInfo BufferInfo;
        MemoryInfo MemoryInfo;
        uint32_t Countdown;
        
        std::vector<TransientBufferObject> Buffers;
    };

    struct TextureResource
    {
        VkImage Image;
        VkImageView ImageView;
        TextureInfo TextureInfo;
        uint32_t TexturePoolIndex;
        uint64_t MemoryOffset;
        uint32_t Id;
        bool PooledResource;
    };

    struct BufferResource
    {
        VkBuffer Buffer;
        BufferInfo BufferInfo;
        uint32_t BufferPoolIndex;
        uint64_t MemoryOffset;
        uint32_t Id;
        bool PooledResource;
    };

    struct MemoryBucket
    {
        VmaAllocation Allocation;
        VmaAllocationInfo AllocationInfo;
        bool IsActive;
    };

    class TransientResourcePool
    {
        public:

            static void AddTextureResource(const TextureInfo& textureInfo, const uint32_t Id, const uint32_t frameIndex);
            static void AddBufferResource(const BufferInfo& bufferInfo, const uint32_t Id, const uint32_t frameIndex);

            inline static TextureResource& GetTextureObject(const uint32_t index, const uint32_t frameIndex) { return transientTextures[frameIndex][index]; }
            inline static BufferResource& GetBufferObject(const uint32_t index, const uint32_t frameIndex) { return transientBuffers[frameIndex][index]; }

            inline static TexturePool& GetTexturePool(const uint32_t poolIndex) { return texturePools[poolIndex]; }
            inline static BufferPool& GetBufferPool(const uint32_t poolIndex) { return bufferPools[poolIndex]; }

            inline static std::vector<TexturePool>& GetTexturePools() { return texturePools; }
            inline static std::vector<BufferPool>& GetBufferPools() { return bufferPools; }

            static uint32_t FindTexturePoolIndex(const TextureInfo& textureInfo, const uint32_t passesCount);
            static uint32_t FindBufferPoolIndex(const BufferInfo& bufferInfo, const uint32_t passesCount);

            inline static MemoryBucket& GetTextureMemoryBucket(const uint32_t bucketIndex) 
            { return texturesMemoryBucket[bucketIndex]; }
            inline static MemoryBucket& GetBufferMemoryBucket(const uint32_t bucketIndex) 
            { return buffersMemoryBucket[bucketIndex]; }

            static void UpdateTexturesPool(const uint32_t frameIndex);
            static void UpdateBuffersPool(const uint32_t frameIndex);

            static bool ResizeTextureMemoryBucketIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment);
            static bool ResizeBufferMemoryBucketIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment);

        private:

            static uint32_t GetTexturesBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount);
            static uint32_t GetBuffersBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount);

            inline static uint32_t GetTextureMemoryTypeIndex(const uint32_t bucketIndex) { return texturesMemoryTypeIndicies[bucketIndex]; }
            inline static uint32_t GetBufferMemoryTypeIndex(const uint32_t bucketIndex) { return buffersMemoryTypeIndicies[bucketIndex]; }

            inline static std::array<std::vector<TextureResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientTextures;
            inline static std::array<std::vector<BufferResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientBuffers;

            inline static std::vector<TexturePool> texturePools;
            inline static std::vector<BufferPool> bufferPools;

            inline static std::vector<uint32_t> texturePoolFreeSlots;
            inline static std::vector<uint32_t> bufferPoolFreeSlots;

            // The index of the vector represent the bucket index and
            // its value represent the memory type index
            inline static std::vector<uint32_t> texturesMemoryTypeIndicies;
            inline static std::vector<uint32_t> buffersMemoryTypeIndicies;

            inline static std::vector<MemoryBucket> texturesMemoryBucket;
            inline static std::vector<MemoryBucket> buffersMemoryBucket;

            friend class MemoryBin;
    };
}