#include <Eve/graphics/PassModule.hpp>
#include "MemoryManager.hpp"

using namespace Eve::Graphics;

void GraphicsPass::DrawMesh(MeshHandle mesh, ShaderHandle shader, uint32_t instanceCount)
{
    drawCalls.emplace_back(mesh, shader, instanceCount);
}

void TransferPass::CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    transientBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    buffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    buffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, Vec3Int Extent, Vec3Int SrcOffset, Vec3Int DstOffset)
{
    transientTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    textures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    textures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    transientBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLenght, BufferHeightLenght, DstOffset, Extent);
    buffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    textures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset,
     uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    transientTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLenght, BufferHeightLenght);
    textures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    buffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
}

void TransferPass::UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Data.Size = Size,
        .Data.Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryManager::AllocateHostBuffer(stagingBufferInfo);

    Buffer dstBuffer = MemoryManager::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    // --- Record the upload command to execute ---
    transientBufferUploads.emplace_back(stagingBufferHandle.Id, DstBuffer.Id, Size, DstOffset);
    buffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::UploadTexture(void* SrcData, uint64_t Size,  TransientTextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
    uint32_t BufferRowLenght, uint32_t BufferHeightLenght)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Data.Size = Size,
        .Data.Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
    };

    BufferHandle stagingBufferHandle = MemoryManager::AllocateHostBuffer(stagingBufferInfo);

    Buffer dstBuffer = MemoryManager::GetBuffer(stagingBufferHandle);

    // --- Data copy into the stagin buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    transientTextureUploads.emplace_back(stagingBufferHandle.Id, DstTexture.Id, DstOffset, Extent, BufferRowLenght, BufferHeightLenght);
    textures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

#pragma region Common
void GraphicsPass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    textures.push_back(std::pair{texture, accessType});
}
void GraphicsPass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    buffers.push_back(std::pair{buffer, accessType});
}

void ComputePass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    textures.push_back(std::pair{texture, accessType});
}
void ComputePass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    buffers.push_back(std::pair{buffer, accessType});
}
#pragma endregion