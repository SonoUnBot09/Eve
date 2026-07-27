#include <Eve/graphics/PassModule.hpp>

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

void TransferPass::CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent)
{
    transientBufferToTextureCopies.emplace_back(SrcBuffer.Id, DstTexture.Id, SrcOffset, DstOffset, Extent);
    buffers.push_back(std::pair{SrcBuffer, Usage::COPY_SOURCE});
    textures.push_back(std::pair{DstTexture, Usage::COPY_DESTINATION});
}

void TransferPass::CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset)
{
    transientTextureToBufferCopies.emplace_back(SrcTexture.Id, DstBuffer.Id, SrcOffset, Extent, DstOffset);
    textures.push_back(std::pair{SrcTexture, Usage::COPY_SOURCE});
    buffers.push_back(std::pair{DstBuffer, Usage::COPY_DESTINATION});
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