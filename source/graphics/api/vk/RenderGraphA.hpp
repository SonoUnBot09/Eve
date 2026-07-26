#pragma once

#include "Eve/graphics/Texture.hpp"
#include "graphics/api/vk/ContextBuilder.hpp"
#include <algorithm>
#include <ranges>
#include <vector>
#include <cstdint>

#include <Eve/graphics/PassModule.hpp>
#include <graphics/api/vk/MemoryManager.hpp>
#include <EveSettings.hpp>

using namespace Eve::Graphics;

namespace Eve::Graphics
{
    class RenderGraph
    {
        public:

            static void Initialize();

            static TransientTextureHandle RequestTransientTexture1D(TransientTextureInfo1D textureInfo);
            static TransientTextureHandle RequestTransientTexture2D(TransientTextureInfo2D textureInfo);
            static TransientTextureHandle RequestTransientTexture3D(TransientTextureInfo3D textureInfo);
            static TransientBufferHandle RequestTransientBuffer(TransientBufferInfo bufferInfo);
            static void AddPass(GraphicsPass* pass);
            static void AddPass(TransferPass* pass);
            static void AddPass(ComputePass* pass);

            static void CompileGraph(uint32_t frameIndex);

            struct TextureBarrierInfo
            {
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
                VkImageLayout Layout;
            };

            struct BufferBarrierInfo
            {
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
            };

        private:

            struct TextureBarrierInfoPair
            {
                TextureBarrierInfo SrcInfo;
                TextureBarrierInfo DstInfo;
            };

            struct BufferBarrierInfoPair
            {
                BufferBarrierInfo SrcInfo;
                BufferBarrierInfo DstInfo;
            };
            

            struct Pass
            {
                Pass(
                    std::vector<std::pair<TransientTextureHandle, Usage>>& textures,
                    std::vector<std::pair<TransientBufferHandle, Usage>>& buffers
                ) : Textures(textures), Buffers(buffers) {};
                std::vector<std::pair<TransientTextureHandle, Usage>> Textures;
                std::vector<std::pair<TransientBufferHandle, Usage>> Buffers;

                std::vector<TextureBarrierInfoPair> texturesBarriers;
                std::vector<BufferBarrierInfoPair> buffersBarriers;
            };

            struct TexturesBucketPass
            {
                std::vector<TransientTextureHandle> TexturesToCreate;
                std::vector<TransientTextureHandle> TexturesToDestroy;
            };

            struct BuffersBucketPass
            {
                std::vector<TransientBufferHandle> BuffersToCreate;
                std::vector<TransientBufferHandle> BuffersToDestroy;
            };

            struct TextureResource
            {
                TextureInfo TextureInfo;
                VkImage Image;
                VkImageView ImageView;
                uint32_t FramesCount;
                bool PooledImage;
                
                struct MemoryInfo
                {
                    uint64_t Size;
                    uint64_t Alignment;
                    uint32_t BucketIndex;
                } MemoryInfo;
            };

            struct BufferResource
            {
                BufferInfo BufferInfo;
                VkBuffer Buffer;
                uint32_t FramesCount;
                bool PooledBuffer;
                
                struct MemoryInfo
                {
                    uint64_t Size;
                    uint64_t Alignment;
                    uint32_t BucketIndex;
                } MemoryInfo;
            };

            struct MemoryBucket
            {
                VmaAllocation Allocation;
                VmaAllocationInfo AllocationInfo;
                bool used;
            };

            struct MemorySlot
            {
                uint64_t Start;
                uint64_t End;
                uint32_t ResourceId;
            };

            static uint32_t SetTextureMemoryInfo(const uint32_t frameIndex, const uint32_t textureId, const uint32_t passesCount);
            static uint32_t SetBufferMemoryInfo(const uint32_t frameIndex, const uint32_t bufferId, const uint32_t passesCount);

            static uint32_t GetTexturesBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount);
            static uint32_t GetBuffersBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount);
            static uint32_t GetTexturesMemoryTypeIndex(const uint32_t bucketIndex);
            static uint32_t GetBuffersMemoryTypeIndex(const uint32_t bucketIndex);

            static TextureBarrierInfo GetFirstTextureBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize);
            static BufferBarrierInfo GetFirstBufferBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize);

            static bool ResizeTextureMemoryPoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment);
            static bool ResizeBufferMemoryPoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment);

            static void UpdateTexturesPool(const uint32_t frameIndex);
            static void UpdateBuffersPool(const uint32_t frameIndex);

            // Input
            inline static std::vector<TextureInfo> requestedTextures;
            inline static std::vector<BufferInfo> requestedBuffers;
            inline static std::vector<Pass> passes;

            inline static std::vector<std::vector<TexturesBucketPass>> texturesBucketPasses;
            inline static std::vector<std::vector<BuffersBucketPass>> buffersBucketPasses;

            inline static std::array<std::vector<TextureResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientTextures;
            inline static std::array<std::vector<BufferResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientBuffers;

            inline static std::vector<uint32_t> barriersOffsetPerTexture;
            inline static std::vector<std::pair<TextureBarrierInfo, uint32_t>> texturesBarriersInfo; // The second element in the pair is the sync point index
            inline static std::vector<uint32_t> barriersOffsetPerBuffer;
            inline static std::vector<std::pair<BufferBarrierInfo, uint32_t>> buffersBarriersInfo;  // The second element in the pair is the sync point index

            // The index of the vector represent the bucket index and
            // is value represent the memory type index
            inline static std::vector<uint32_t> texturesMemoryTypeIndicies;
            inline static std::vector<uint32_t> buffersMemoryTypeIndicies;

            // Need to be ordered
            inline static std::vector<std::pair<TextureResource, bool>> texturesPool;
            inline static std::vector<std::pair<BufferResource, bool>> buffersPool;

            // Memory pools
            inline static std::vector<MemoryBucket> texturesMemoryBucket;
            inline static std::vector<MemoryBucket> buffersMemoryBucket;

            // Virtual Allocations
            inline static std::vector<VmaVirtualAllocation> texturesVirtualAllocs;
            inline static std::vector<VmaVirtualAllocation> buffersVirtualAllocs;
            inline static std::vector<MemorySlot> virtualMemorySlots;
    };
}