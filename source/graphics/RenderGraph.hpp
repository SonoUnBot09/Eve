#pragma once

#include "eve/graphics/Buffer.hpp"
#include <vector>
#include <cstdint>

#include <eve/graphics/Pass.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
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
            static TransientTextureHandle RequestTransientTextureCube(TransientTextureInfo2D textureInfo);
            static TransientBufferHandle RequestTransientBuffer(uint64_t size);            
            static void AddPass(GraphicsPass& pass);
            static void AddPass(TransferPass& pass);
            static void AddPass(ComputePass& pass);
            static void AddPass(GraphicsPass& pass, uint32_t index);
            static void AddPass(TransferPass& pass, uint32_t index);
            static void AddPass(ComputePass& pass, uint32_t index);

            static void SetPresentTexture2D(TransientTextureHandle handle);

            static bool Execute(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swapchainImageIndex);

            inline static TransferPass& GetUniversalTransferPass() { return universalTransferPass; }

            struct TextureBarrierInfo
            {
                uint32_t TextureId;
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
                VkImageLayout Layout;
            };

            struct BufferBarrierInfo
            {
                uint32_t BufferId;
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
            };

            struct PersistentTextureState
            {
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
                VkImageLayout Layout;
                Usage Usage;
            };

            struct PersistentBufferState
            {
                VkPipelineStageFlags2 StageMask;
                VkAccessFlags2 AccessMask;
                Usage Usage;
            };

            static void AddTextureBucketPasses(uint32_t passesCount);
            static void AddBufferBucketPasses(uint32_t passesCount);

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

                enum class PassType : uint32_t
                {
                    GRAPHICS,
                    TRANSFER,
                    COMPUTE
                };

                PassType passType;

                std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;
                std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;

                std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;
                std::vector<std::pair<TextureHandle, Usage>> persistentTextures;

                std::vector<std::pair<TransientTextureHandle, LoadStoreOp>> loadStoreOps;

                // --- Graphics ---
                std::vector<DrawCall> drawCalls;
                std::vector<InstanceParams> instanceParams;

                // --- Transfer ---
                std::vector<BufferCopy> transientBufferCopies;
                std::vector<TextureCopy> transientTextureCopies;
                std::vector<BufferToTextureCopy> transientBufferToTextureCopies;
                std::vector<TextureToBufferCopy> transientTextureToBufferCopies;

                std::vector<BufferCopy> persistentBufferCopies;
                std::vector<TextureCopy> persistentTextureCopies;
                std::vector<BufferToTextureCopy> persistentBufferToTextureCopies;
                std::vector<TextureToBufferCopy> persistentTextureToBufferCopies;

                std::vector<BufferCopy> transientPersistentBufferCopies;
                std::vector<BufferCopy> persistentTransientBufferCopies;
                std::vector<TextureCopy> transientPersistentTextureCopies;
                std::vector<TextureCopy> persistentTransientTextureCopies;

                std::vector<BufferToTextureCopy> transientPersistentBufferToTextureCopies;
                std::vector<BufferToTextureCopy> persistentTransientBufferToTextureCopies;
                std::vector<TextureToBufferCopy> transientPersistentTextureToBufferCopies;
                std::vector<TextureToBufferCopy> persistentTransientTextureToBufferCopies;

                std::vector<BufferUpload> transientBufferUploads;
                std::vector<TextureUpload> transientTextureUploads;
                std::vector<BufferUpload> persistentBufferUploads;
                std::vector<TextureUpload> persistentTextureUploads;

                // --- Compute ---


                std::vector<TextureBarrierInfoPair> transientTexturesBarriers;
                std::vector<BufferBarrierInfoPair> transientBuffersBarriers;
                std::vector<TextureBarrierInfoPair> persistentTexturesBarriers;
                std::vector<BufferBarrierInfoPair> persistentBuffersBarriers;
                
            };

            struct TexturesBucketPass
            {
                std::vector<uint32_t> TexturesToCreate;
                std::vector<uint32_t> TexturesToDestroy;
            };

            struct BuffersBucketPass
            {
                std::vector<uint32_t> BuffersToCreate;
                std::vector<uint32_t> BuffersToDestroy;
            };

            struct PushConstant
            {
                uint64_t DrawInfoParamsBufferOffset;
                uint32_t GlobalInstanceOffsetID;
                uint32_t MaterialBufferID;
                uint32_t InstanceParamsBufferID;
                uint32_t DrawCallInfoParamsBufferID;
                uint32_t RenderViewBufferID;
                uint32_t RenderViewID;

                bool operator== (PushConstant& other)
                {
                    return
                        DrawInfoParamsBufferOffset == other.DrawInfoParamsBufferOffset &&
                        GlobalInstanceOffsetID == other.GlobalInstanceOffsetID &&
                        MaterialBufferID == other.MaterialBufferID &&
                        InstanceParamsBufferID == other.InstanceParamsBufferID &&
                        DrawCallInfoParamsBufferID == other.DrawCallInfoParamsBufferID &&
                        RenderViewBufferID == other.RenderViewBufferID &&
                        RenderViewID == other.RenderViewID;
                };
            };

            struct MemorySlot
            {
                uint64_t Start;
                uint64_t End;
                uint32_t ResourceIndex;
            };

            static bool CompileGraph(uint32_t frameIndex);
            static bool RecordCommands(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swaphchainImageIndex);
            static void Clear();

            static uint32_t SetTextureMemoryInfo(const uint32_t frameIndex, const uint32_t textureId, const uint32_t passesCount);
            static uint32_t SetBufferMemoryInfo(const uint32_t frameIndex, const uint32_t bufferId, const uint32_t passesCount);

            static TextureBarrierInfo GetFirstTextureBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize);
            static BufferBarrierInfo GetFirstBufferBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize);

            // --- Commands Recorders ---
            static void RecordTransientBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);

            static void RecordPersistentBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);

            static void RecordTransientPersistentBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTransientBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientPersistentTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTransientTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            
            static void RecordTransientPersistentBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTransientBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientPersistentTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTransientTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);

            static void RecordTransientBufferUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordTransientTextureUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);

            static void RecordPersistentBufferUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordPersistentTextureUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);

            static void RecordDrawCalls(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex);
            static void RecordSwapchainDrawingPass(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swapchainImageIndex);

            static void UploadInstanceAnDrawInfoParams();
            static void UploadRenderViews();

            // Input
            inline static std::vector<TextureInfo> transientRequestedTextures;
            inline static std::vector<TransientTextureHandle> transientRequestedTextureHandles;

            inline static std::vector<BufferInfo> transientRequestedBuffers;
            inline static std::vector<TransientBufferHandle> transientRequestedBufferHandles;

            inline static std::vector<Pass> passes;

            inline static std::vector<uint32_t> transientTextureHandleToIndex;
            inline static std::vector<uint32_t> transientBufferHandleToIndex;

            inline static std::vector<std::vector<TexturesBucketPass>> texturesBucketPasses;
            inline static std::vector<std::vector<BuffersBucketPass>> buffersBucketPasses;
            
            inline static TransientTextureHandle presentTexture = {UINT32_MAX};
            inline static bool isPresentTextureValid = false;

            inline static TransferPass universalTransferPass;
            inline static std::vector<InstanceParams> instanceParams;
            inline static std::vector<std::byte> drawCallInfoParams;
            inline static TransientBufferHandle instanceParamsBuffer;
            inline static TransientBufferHandle drawInfoParamsBuffer;
            inline static TransientBufferHandle renderViewsBuffer;
            inline static uint32_t GlobalInstanceOffsetID;
            inline static uint64_t GlobalDrawInfoParamsOffset;

            inline static std::vector<uint32_t> barriersOffsetPerTexture;
            inline static std::vector<std::pair<TextureBarrierInfo, uint32_t>> texturesBarriersInfo; // The second element in the pair is the sync point index
            inline static std::vector<uint32_t> barriersOffsetPerBuffer;
            inline static std::vector<std::pair<BufferBarrierInfo, uint32_t>> buffersBarriersInfo;  // The second element in the pair is the sync point index

            // Virtual Allocations
            inline static std::vector<VmaVirtualAllocation> texturesVirtualAllocs;
            inline static std::vector<VmaVirtualAllocation> buffersVirtualAllocs;
            inline static std::vector<MemorySlot> virtualMemorySlots;

            friend class MemoryRegistry;
    };
}