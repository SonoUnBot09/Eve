#pragma once

#include <algorithm>
#include <ranges>
#include <vector>
#include <cstdint>

#include <Eve/graphics/PassModule.hpp>
#include <graphics/MemoryManager.hpp>
#include <EveSettings.hpp>

using namespace Eve::Graphics;

namespace Eve::Graphics
{
    class RenderGraph
    {
        public:

            static void Initialize();
            static TransientTextureHandle RequestNewTransientTexture1D(TransientTextureInfo1D textureInfo);
            static TransientTextureHandle RequestNewTransientTexture2D(TransientTextureInfo2D textureInfo);
            static TransientTextureHandle RequestNewTransientTexture3D(TransientTextureInfo3D textureInfo);
            static TransientBufferHandle RequestNewTransientBuffer(TransientBufferInfo bufferInfo);
            static void AddPass(GraphicsPass* pass);
            static void AddPass(TransferPass* pass);
            static void AddPass(ComputePass* pass);

            static void CompileGraph(uint32_t frameIndex);

        private:

            struct Pass
            {
                Pass(
                    std::vector<std::pair<TransientTextureHandle, Usage>>& textures,
                    std::vector<std::pair<TransientBufferHandle, Usage>>& buffers
                ) : Textures(textures), Buffers(buffers) {};
                std::vector<std::pair<TransientTextureHandle, Usage>> Textures;
                std::vector<std::pair<TransientBufferHandle, Usage>> Buffers;

                std::vector<TransientTextureHandle> TexturesToCreate;
                std::vector<TransientTextureHandle> TexturesToDestroy;
                std::vector<TransientBufferHandle> BuffersToCreate;
                std::vector<TransientBufferHandle> BuffersToDestroy;
            };

            struct SyncPoint
            {
                std::vector<VkImageMemoryBarrier2> imageBarriers;
                std::vector<VkBufferMemoryBarrier2> bufferBarriers;
            };

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

            struct TextureBarrierInfoPair
            {
                TextureBarrierInfo SrcInfo;
                TextureBarrierInfo DstInfo;
                uint32_t SyncPointIndex;
            };

            struct BufferBarrierInfoPair
            {
                BufferBarrierInfo SrcInfo;
                BufferBarrierInfo DstInfo;
                uint32_t SyncPointIndex;
            };
            
            struct TextureResource
            {
                TextureInfo TextureInfo;
                Texture Texture;
                uint32_t FramesCount;
            };

            struct BufferResource
            {
                BufferInfo BufferInfo;
                Buffer Buffer;
                uint32_t FramesCount;
            };

            struct MemorySlot
            {
                uint64_t Start;
                uint64_t End;
                uint64_t ResourceId;
            };

            static Texture AllocateTransientTexture(TextureInfo textureInfo);
            static Buffer AllocateTransientBuffer(BufferInfo bufferInfo);
            static TextureBarrierInfo CalculateTextureBarrierInfo(Usage usage);
            static BufferBarrierInfo CalculateBufferBarrierInfo(Usage usage);

            static TextureUsage GetTextureUsage(Usage usage);
            static BufferUsage GetBufferUsage(Usage usage);
            static bool IsReadOnly(Usage usage);

            static VkImageCreateInfo GetVirtualTextureCreateInfo(TextureInfo textureInfo);
            static VkBufferCreateInfo GetVirtualBufferCreateInfo(BufferInfo bufferInfo);

            inline static std::vector<TextureInfo> requestedTextures;
            inline static std::vector<BufferInfo> requestedBuffers;

            inline static std::array<std::vector<TextureResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientTextures;
            inline static std::array<std::vector<BufferResource>, Eve::Settings::MAX_FRAMES_IN_FLIGHT> transientBuffers;
            inline static std::vector<std::vector<TextureBarrierInfoPair>> textureBarriers;
            inline static std::vector<std::vector<BufferBarrierInfoPair>> bufferBarriers;
            inline static std::vector<TextureBarrierInfo> lastUsageTextures;
            inline static std::vector<BufferBarrierInfo> lastUsageBuffers;
            
            inline static std::vector<std::pair<TextureResource, bool>> texturesToReuse;
            inline static std::vector<std::pair<BufferResource, bool>> buffersToReuse;

            inline static std::vector<Pass> passes;
            inline static std::vector<SyncPoint> syncPoints;

            // Pools
            inline static uint64_t texturePoolSize = Eve::Settings::transientDefaultTexturesPoolSize;
            inline static uint64_t bufferPoolSize = Eve::Settings::transientDefaultBuffersPoolSize;
            inline static std::vector<VmaPool> texturePoolsInUse;
            inline static std::vector<VmaPool> bufferPoolsInUse;
            inline static std::vector<std::pair<VmaPool, uint32_t>> poolsToDestroy;

            inline static std::vector<VmaVirtualAllocation> textureVirtualAllocations;
            inline static std::vector<VmaVirtualAllocation> bufferVirtualAllocations;

            inline static std::vector<MemorySlot> textureMemorySlots;
            inline static std::vector<MemorySlot> bufferMemorySlots;

            inline static std::vector<uint32_t> textureOccupancyCount;
            inline static std::vector<uint32_t> bufferOccupancyCount;
            inline static std::vector<TransientTextureHandle> usedTextureMemorySlotIds;
            inline static std::vector<TransientBufferHandle> usedBufferMemorySlotIds;
    };
}