#pragma once

#include "Eve/graphics/Texture.hpp"
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
                std::vector<TransientTextureHandle> BuffersToCreate;
                std::vector<TransientTextureHandle> BuffersToDestroy;
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
            

            static TextureBarrierInfo CalculateTextureBarrierInfo(Usage usage);
            static BufferBarrierInfo CalculateBufferBarrierInfo(Usage usage);
            static TextureUsage GetTextureUsage(Usage usage);
            static BufferUsage GetBufferUsage(Usage usage);
            static bool IsOnlyRead(Usage usage);

            inline static std::vector<TextureInfo> requestedTextures;
            inline static std::vector<BufferInfo> requestedBuffers;

            inline static std::vector<std::array<TextureHandle, Eve::Settings::MAX_FRAMES_IN_FLIGHT>> transientTextures;
            inline static std::vector<std::array<BufferHandle, Eve::Settings::MAX_FRAMES_IN_FLIGHT>> transientBuffers;
            
            inline static std::vector<Pass> passes;
            inline static std::vector<SyncPoint> syncPoints;

            // Pools
            inline static uint64_t texturePoolSize = Eve::Settings::transientDefaultTexturesPoolSize;
            inline static uint64_t bufferPoolSize = Eve::Settings::transientDefaultBuffersPoolSize;
            inline static std::vector<VmaPool> texturePoolsInUse;
            inline static std::vector<VmaPool> bufferPoolsInUse;
            inline static std::vector<std::pair<VmaPool, uint32_t>> poolsToDestroy;
    };
}