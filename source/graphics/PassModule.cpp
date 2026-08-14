#include <Eve/graphics/PassModule.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include "registers/MemoryRegistry.hpp"

using namespace Eve::Graphics;

void GraphicsPass::Draw(uint32_t vertexShaderInvocations, ShaderHandle shader, const void* pushConstant, Words32 offset, Words32 size)
{
    static constexpr uint32_t maxPushCostantSize = 128;

    uint32_t offsetBytes = offset.ToBytes();
    uint32_t sizeBytes = size.ToBytes();

    if(offsetBytes > maxPushCostantSize)
    {
        offsetBytes = maxPushCostantSize;
    }

    uint32_t availableSpace = maxPushCostantSize - offsetBytes;

    if(sizeBytes > availableSpace)
    {
        sizeBytes = availableSpace;
    }

    std::array<std::byte, maxPushCostantSize> pushConstantData;

    memcpy(pushConstantData.data() + sizeBytes, (std::byte*)pushConstant, sizeBytes);
    
    drawCalls.emplace_back(vertexShaderInvocations, 1, shader, pushConstantData, offset, size);
}

void TransferPass::CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    transientBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    transientBuffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    transientBuffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::CopyBuffer(BufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    persistentBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    persistentBuffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    persistentBuffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, Vec3Int Extent, Vec3Int SrcOffset, Vec3Int DstOffset)
{
    transientTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    transientTextures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    transientTextures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTexture(TextureHandle SrcTexture, TextureHandle DstTexture, Vec3Int Extent, Vec3Int SrcOffset, Vec3Int DstOffset)
{
    persistentTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    persistentTextures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    persistentTextures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    transientBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLenght, BufferHeightLenght, DstOffset, Extent);
    transientBuffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    transientTextures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    persistentBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLenght, BufferHeightLenght, DstOffset, Extent);
    persistentBuffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    persistentTextures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset,
     uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    transientTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLenght, BufferHeightLenght);
    transientTextures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    transientBuffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset,
     uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    persistentTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLenght, BufferHeightLenght);
    persistentTextures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    persistentBuffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);

    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    // --- Record the upload command to execute ---
    transientBufferUploads.emplace_back(stagingBufferHandle.Id, DstBuffer.Id, Size, DstOffset);
    transientBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::UploadBuffer(void* SrcData, BufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);

    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    // --- Record the upload command to execute ---
    persistentBufferUploads.emplace_back(stagingBufferHandle.Id, DstBuffer.Id, Size, DstOffset);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::UploadTexture(void* SrcData, uint64_t Size,  TransientTextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    
    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    transientTextureUploads.emplace_back(stagingBufferHandle.Id, DstTexture.Id, DstOffset, Extent, BufferRowLenght, BufferHeightLenght);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::UploadTexture(void* SrcData, uint64_t Size,  TextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    
    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    persistentTextureUploads.emplace_back(stagingBufferHandle.Id, DstTexture.Id, DstOffset, Extent, BufferRowLenght, BufferHeightLenght);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

#pragma region Common
void GraphicsPass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    transientTextures.push_back(std::pair{texture, accessType});
}
void GraphicsPass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    transientBuffers.push_back(std::pair{buffer, accessType});
}

void GraphicsPass::UseColorTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)
{
    UseTransientTexture(texture, Usage::COLOR_ATTACHMENT);

    loadStoreOps.push_back(std::pair{texture,loadStoreOp});
}

void GraphicsPass::UseDepthStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)
{
    UseTransientTexture(texture, Usage::DEPTH_STENCIL);

    loadStoreOps.push_back(std::pair{texture,loadStoreOp});
}

void GraphicsPass::UseDepthTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)
{
    UseTransientTexture(texture, Usage::DEPTH);

    loadStoreOps.push_back(std::pair{texture,loadStoreOp});
}

void GraphicsPass::UseStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)
{
    UseTransientTexture(texture, Usage::STENCIL);

    loadStoreOps.push_back(std::pair{texture,loadStoreOp});
}

void ComputePass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    transientTextures.push_back(std::pair{texture, accessType});
}
void ComputePass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    transientBuffers.push_back(std::pair{buffer, accessType});
}
#pragma endregion