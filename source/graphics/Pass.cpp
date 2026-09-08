#include <eve/graphics/Pass.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include <eve/graphics/details/Usage.hpp>
#include "eve/graphics/Buffer.hpp"
#include "eve/graphics/MaterialHandle.hpp"
#include "eve/graphics/RenderViewHandle.hpp"
#include "registers/MemoryRegistry.hpp"
#include <algorithm>

using namespace Eve::Graphics;

#pragma region Graphics Pass

void GraphicsPass::Draw(uint32_t vertexShaderInvocations, const Transform& transformParam, MaterialHandle material, RenderViewHandle renderView, DrawParams* drawParams)
{
    uint64_t drawParamsOffset = 0;
    uint32_t drawParamsSizeBytes = 0;

    if(drawParams != nullptr)
    {
        static constexpr uint32_t maxDrawInfoSize = 16 * 1024;
        drawParamsSizeBytes = std::min((drawParams->SizeBytes + 7u) & ~7u, maxDrawInfoSize);

        drawParamsOffset = drawCallParams.size();

        drawCallParams.resize(drawParamsOffset + drawParamsSizeBytes);

        memcpy(drawCallParams.data() + drawParamsOffset, drawParams->Data, drawParamsSizeBytes);
    }

    drawCalls.emplace_back(vertexShaderInvocations, 1, material, drawParamsSizeBytes, renderView);

    transforms.push_back(transformParam);
}

void GraphicsPass::DrawInstanced(uint32_t vertexShaderInvocations, uint32_t instanceCount, const Transform* transformsParams, MaterialHandle material, RenderViewHandle renderView, DrawParams* drawParams)
{
    uint64_t drawParamsOffset = 0;
    uint32_t drawParamsSizeBytes = 0;

    if(drawParams != nullptr)
    {
        static constexpr uint32_t maxDrawInfoSize = 16 * 1024;
        drawParamsSizeBytes = std::min((drawParams->SizeBytes + 7u) & ~7u, maxDrawInfoSize);

        drawParamsOffset = drawCallParams.size();

        drawCallParams.resize(drawParamsOffset + drawParamsSizeBytes);

        memcpy(drawCallParams.data() + drawParamsOffset, drawParams->Data, drawParamsSizeBytes);
    }

    drawCalls.emplace_back(vertexShaderInvocations, instanceCount, material, drawParamsSizeBytes, renderView);

    size_t startOffset = transforms.size();
    transforms.resize(startOffset + instanceCount);

    memcpy
    (
        transforms.data() + startOffset,
        transformsParams,
        sizeof(Transform) * instanceCount
    );
}

void GraphicsPass::UseTextureVertex(TransientTextureHandle texture)
{
    transientTextures.emplace_back(texture, Usage::VERTEX_READ_TEXTURE_SAMPLED);
}
void GraphicsPass::UseTextureFragment(TransientTextureHandle texture)
{
    transientTextures.emplace_back(texture, Usage::FRAGMENT_READ_TEXTURE_SAMPLED);
}
void GraphicsPass::UseTextureVertexFragment(TransientTextureHandle texture)
{
    transientTextures.emplace_back(texture, Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED);
}

void GraphicsPass::UseTextureVertex(TextureHandle texture)
{
    persistentTextures.emplace_back(texture, Usage::VERTEX_READ_TEXTURE_SAMPLED);
}
void GraphicsPass::UseTextureFragment(TextureHandle texture)
{
    persistentTextures.emplace_back(texture, Usage::FRAGMENT_READ_TEXTURE_SAMPLED);
}
void GraphicsPass::UseTextureVertexFragment(TextureHandle texture)
{
    persistentTextures.emplace_back(texture, Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED);
}

void GraphicsPass::UseBufferReadOnlyVertex(BufferHandle buffer)
{
    persistentBuffers.emplace_back(buffer, Usage::VERTEX_READ_BUFFER_STORAGE);
}
void GraphicsPass::UseBufferReadOnlyFragment(BufferHandle buffer)
{
    persistentBuffers.emplace_back(buffer, Usage::FRAGMENT_READ_BUFFER_STORAGE);
}
void GraphicsPass::UseBufferReadOnlyVertexFragment(BufferHandle buffer)
{
    persistentBuffers.emplace_back(buffer, Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE);
}

void GraphicsPass::UseBufferReadOnlyVertex(TransientBufferHandle buffer)
{
    transientBuffers.emplace_back(buffer, Usage::VERTEX_READ_BUFFER_STORAGE);
}
void GraphicsPass::UseBufferReadOnlyFragment(TransientBufferHandle buffer)
{
    transientBuffers.emplace_back(buffer, Usage::FRAGMENT_READ_BUFFER_STORAGE);
}
void GraphicsPass::UseBufferReadOnlyVertexFragment(TransientBufferHandle buffer)
{
    transientBuffers.emplace_back(buffer, Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE);
}

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

void GraphicsPass::Clear()
{
    transientTextures.clear();
    transientBuffers.clear();

    persistentTextures.clear();
    persistentBuffers.clear();

    loadStoreOps.clear();

    drawCalls.clear();
    transforms.clear();
}
#pragma endregion

#pragma region Transfer Pass

void TransferPass::CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    transientBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    transientBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    transientBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::CopyBuffer(BufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    persistentBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    persistentBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset, glm::ivec3 DstOffset)
{
    transientTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    transientTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::CopyTexture(TextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset, glm::ivec3 DstOffset)
{
    persistentTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    persistentTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    transientBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLength, BufferHeightLength, DstOffset, Extent);
    transientBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    persistentBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLength, BufferHeightLength, DstOffset, Extent);
    persistentBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset,
     uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    transientTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLength, BufferHeightLength);
    transientTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    transientBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset,
     uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    persistentTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLength, BufferHeightLength);
    persistentTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

// Buffer Mix
void TransferPass::CopyBuffer(TransientBufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    transientPersistentBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    transientBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}
void TransferPass::CopyBuffer(BufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset, uint64_t DstOffset)
{
    persistentTransientBufferCopies.emplace_back(SrcBuffer.Id, DstBuffer.Id, Size, SrcOffset, DstOffset);
    persistentBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    transientBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

// Texture Mix
void TransferPass::CopyTexture(TransientTextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset, glm::ivec3 DstOffset)
{
    transientPersistentTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    transientTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}
void TransferPass::CopyTexture(TextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset, glm::ivec3 DstOffset)
{
    persistentTransientTextureCopies.emplace_back(SrcTexture.Id, DstTexture.Id, Extent, SrcOffset, DstOffset);
    persistentTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

// Buffer To Texture Mix
void TransferPass::CopyBufferToTexture(TransientBufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    transientPersistentBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLength, BufferHeightLength, DstOffset, Extent);
    transientBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}
void TransferPass::CopyBufferToTexture(BufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    persistentTransientBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, BufferRowLength, BufferHeightLength, DstOffset, Extent);
    persistentBuffers.emplace_back(SrcBuffer, Usage::COPY_SOURCE);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

// Texture To Buffer Mix
void TransferPass::CopyTextureToBuffer(TransientTextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    transientPersistentTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLength, BufferHeightLength);
    transientTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}
void TransferPass::CopyTextureToBuffer(TextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    persistentTransientTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset, BufferRowLength, BufferHeightLength);
    persistentTextures.emplace_back(SrcTexture, Usage::COPY_SOURCE);
    transientBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    BufferObject srcBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the staging buffer ---
    memcpy(srcBuffer.AllocationInfo.pMappedData, SrcData, Size);

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
        .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    BufferObject srcBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the staging buffer ---
    memcpy(srcBuffer.AllocationInfo.pMappedData, SrcData, Size);
    
    // --- Record the upload command to execute ---
    persistentBufferUploads.emplace_back(stagingBufferHandle.Id, DstBuffer.Id, Size, DstOffset);
    persistentBuffers.emplace_back(DstBuffer, Usage::COPY_DESTINATION);
}

void TransferPass::UploadTexture(void* SrcData, uint64_t Size,  TransientTextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the staging buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    transientTextureUploads.emplace_back(stagingBufferHandle.Id, DstTexture.Id, DstOffset, Extent, BufferRowLength, BufferHeightLength);
    transientTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::UploadTexture(void* SrcData, uint64_t Size,  TextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent,
    uint32_t BufferRowLength, uint32_t BufferHeightLength)
{
    // --- Staging buffer creation ---
    BufferInfo stagingBufferInfo
    {
        .Size = Size,
        .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    BufferHandle stagingBufferHandle = MemoryRegistry::CreateCPUBuffer(stagingBufferInfo);
    BufferObject dstBuffer = MemoryRegistry::GetBuffer(stagingBufferHandle);

    // --- Data copy into the staging buffer ---
    memcpy(dstBuffer.AllocationInfo.pMappedData, SrcData, Size);

    persistentTextureUploads.emplace_back(stagingBufferHandle.Id, DstTexture.Id, DstOffset, Extent, BufferRowLength, BufferHeightLength);
    persistentTextures.emplace_back(DstTexture, Usage::COPY_DESTINATION);
}

void TransferPass::Clear()
{
    transientTextures.clear();
    transientBuffers.clear();
    persistentTextures.clear();
    persistentBuffers.clear();

    transientBufferCopies.clear();
    transientTextureCopies.clear();
    transientBufferToTextureCopies.clear();
    transientTextureToBufferCopies.clear();

    persistentBufferCopies.clear();
    persistentTextureCopies.clear();
    persistentBufferToTextureCopies.clear();
    persistentTextureToBufferCopies.clear();

    transientPersistentBufferCopies.clear();
    persistentTransientBufferCopies.clear();
    transientPersistentTextureCopies.clear();
    persistentTransientTextureCopies.clear();

    transientPersistentBufferToTextureCopies.clear();
    persistentTransientBufferToTextureCopies.clear();
    transientPersistentTextureToBufferCopies.clear();
    persistentTransientTextureToBufferCopies.clear();

    transientBufferUploads.clear();
    transientTextureUploads.clear();
    persistentBufferUploads.clear();
    persistentTextureUploads.clear();
}

#pragma endregion

#pragma region Compute Pass

void ComputePass::Dispatch(uint32_t XNumGroups, uint32_t YNumGroups, uint32_t ZNumGroups, ComputeShaderHandle shader, ComputeParams* params)
{
    uint64_t computeParamsOffset = 0;
    uint32_t computeParamsSizeBytes = 0;

    if(params != nullptr)
    {
        static constexpr uint32_t maxDrawInfoSize = 16 * 1024;
        computeParamsSizeBytes = std::min((params->SizeBytes + 7u) & ~7u, maxDrawInfoSize);

        computeParamsOffset = computeParams.size();

        computeParams.resize(computeParamsOffset + computeParamsSizeBytes);

        memcpy(computeParams.data() + computeParamsOffset, params->Data, computeParamsSizeBytes);
    }

    dispatches.emplace_back(
        ComputeDispatch
        {
            XNumGroups,
            YNumGroups,
            ZNumGroups,
            shader,
            computeParamsSizeBytes
        }
    );

}

void ComputePass::UseReadOnlyTexture(TransientTextureHandle texture)
{
    transientTextures.emplace_back(texture, Usage::COMPUTE_READ_TEXTURE_STORAGE);
}
void ComputePass::UseReadOnlyTexture(TextureHandle texture)
{
    persistentTextures.emplace_back(texture, Usage::COMPUTE_READ_TEXTURE_STORAGE);
}

void ComputePass::UseReadWriteTexture(TransientTextureHandle texture)
{
    transientTextures.emplace_back(texture, Usage::COMPUTE_READ_WRITE_TEXTURE_STORAGE);
}
void ComputePass::UseReadWriteTexture(TextureHandle texture)
{
    persistentTextures.emplace_back(texture, Usage::COMPUTE_READ_WRITE_TEXTURE_STORAGE);
}

void ComputePass::UseReadOnlyBuffer(TransientBufferHandle buffer)
{
    transientBuffers.emplace_back(buffer, Usage::COMPUTE_READ_BUFFER_STORAGE);
}
void ComputePass::UseReadOnlyBuffer(BufferHandle buffer)
{
    persistentBuffers.emplace_back(buffer, Usage::COMPUTE_READ_BUFFER_STORAGE);
}

void ComputePass::UseReadWriteBuffer(TransientBufferHandle buffer)
{
    transientBuffers.emplace_back(buffer, Usage::COMPUTE_READ_WRITE_BUFFER_STORAGE);
}
void ComputePass::UseReadWriteBuffer(BufferHandle buffer)
{
    persistentBuffers.emplace_back(buffer, Usage::COMPUTE_READ_WRITE_BUFFER_STORAGE);
}

void ComputePass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    transientTextures.push_back(std::pair{texture, accessType});
}
void ComputePass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    transientBuffers.push_back(std::pair{buffer, accessType});
}

void ComputePass::Clear()
{
    transientTextures.clear();
    transientBuffers.clear();
    persistentTextures.clear();
    persistentBuffers.clear();
}

#pragma endregion