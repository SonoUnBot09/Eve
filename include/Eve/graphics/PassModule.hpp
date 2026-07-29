#pragma once

#include "Eve/utils/Vec.hpp"
#include <vector>
#include <utility>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Buffer.hpp>
#include <Eve/graphics/Mesh.hpp>
#include <Eve/graphics/ShaderHandle.hpp>

namespace Eve::Graphics
{

    enum class Usage
    {
        // Sampled
        VERTEX_READ_TEXTURE_SAMPLED,
        FRAGMENT_READ_TEXTURE_SAMPLED,
        VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED,

        // Storage
        VERTEX_READ_TEXTURE_STORAGE,
        FRAGMENT_READ_TEXTURE_STORAGE,
        VERTEX_FRAGMENT_READ_TEXTURE_STORAGE,
        COMPUTE_READ_TEXTURE_STORAGE,
        COMPUTE_WRITE_TEXTURE_STORAGE,

        // Buffers
        BUFFER_INDEX_READ_ONLY,
        VERTEX_READ_BUFFER_STORAGE,
        VERTEX_READ_BUFFER_UNIFORM,
        FRAGMENT_READ_BUFFER_STORAGE,
        FRAGMENT_READ_BUFFER_UNIFORM,
        VERTEX_FRAGMENT_READ_BUFFER_STORAGE,
        VERTEX_FRAGMENT_READ_BUFFER_UNIFORM,
        COMPUTE_READ_BUFFER_STORAGE,
        COMPUTE_READ_BUFFER_UNIFORM,
        COMPUTE_WRITE_BUFFER_STORAGE,

        // Color Depth Stencil
        COLOR_ATTACHMENT,
        DEPTH_STENCIL_READ_ONLY,
        DEPTH_STENCIL_WRITE,
       
        // Transfer
        COPY_SOURCE,
        COPY_DESTINATION
    };

    struct DrawCall
    {
        MeshHandle MeshHandle;
        ShaderHandle ShaderHandle;
        uint32_t instanceCount;
    };

    struct BufferCopy
    {
        uint32_t SrcBuffer;
        uint32_t DstBuffer;
        uint64_t Size;
        uint64_t SrcOffset;
        uint64_t DstOffset;
    };

    struct TextureCopy
    {
        uint32_t SrcTexture;
        uint32_t DstTexture;
        Vec3Int Extent;
        Vec3Int SrcOffset;
        Vec3Int DstOffset;
    };

    struct BufferToTextureCopy
    {
        uint32_t SrcBuffer; 
        uint32_t DstTexture; 
        uint64_t SrcOffset;
        uint32_t BufferRowLenght;
        uint32_t BufferHeightLenght;
        Vec3Int DstOffset;
        Vec3Int Extent;
    };

    struct TextureToBufferCopy
    {
        uint32_t SrcTexture; 
        uint32_t DstBuffer; 
        Vec3Int SrcOffset;
        Vec3Int Extent;
        uint64_t DstOffset;
        uint32_t BufferRowLenght;
        uint32_t BufferHeightLenght;
    };

    struct BufferUpload
    {
        uint32_t SrcBufferId;
        uint32_t DstBuffer;
        uint64_t Size;
        uint64_t DstOffset;
    };

    struct TextureUpload
    {
        uint32_t SrcBufferId; 
        uint32_t DstTexture; 
        Vec3Int DstOffset;
        Vec3Int Extent;
        uint32_t BufferRowLenght;
        uint32_t BufferHeightLenght;
    };

    struct GraphicsPass
    {
        public:
            void UseTransientTexture(TransientTextureHandle texture, Usage accessType);
            void UseTransientBuffer(TransientBufferHandle buffer, Usage accessType);
            void DrawMesh(MeshHandle mesh, ShaderHandle shader, uint32_t instanceCount);
            inline std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures() { return textures; }
            inline std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers() { return buffers; }
            inline std::vector<DrawCall>& GetDrawCalls() { return drawCalls; }
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
            std::vector<DrawCall> drawCalls;
    };

    struct TransferPass
    {
        public:

            // Transient
            void CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);
            void CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, Vec3Int Extent, Vec3Int SrcOffset = {0,0,0}, Vec3Int DstOffset = {0,0,0});
            void CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);
            void CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset, 
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);

            // Persistent
            void CopyBuffer(BufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);
            void CopyTexture(TextureHandle SrcTexture, TextureHandle DstTexture, Vec3Int Extent, Vec3Int SrcOffset = {0,0,0}, Vec3Int DstOffset = {0,0,0});
            void CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent);
            void CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset);

            // Transient
            void UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TransientTextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);

            // Persistent
            void UploadBuffer(void* SrcData, BufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);
            
            inline std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures() { return textures; }
            inline std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers() { return buffers; }

            inline std::vector<BufferCopy>& GetTransientBufferCopies() { return transientBufferCopies; }
            inline std::vector<TextureCopy>& GetTransientTextureCopies() { return transientTextureCopies; }
            inline std::vector<BufferToTextureCopy>& GetTransientBufferToTextureCopies() { return transientBufferToTextureCopies; }
            inline std::vector<TextureToBufferCopy>& GetTransientTextureToBufferCopies() { return transientTextureToBufferCopies; }

            inline std::vector<BufferCopy>& GetPersistentBufferCopies() { return persistentBufferCopies; }
            inline std::vector<TextureCopy>& GetPersistentTextureCopies() { return persistentTextureCopies; }
            inline std::vector<BufferToTextureCopy>& GetPersistentBufferToTextureCopies() { return persistentBufferToTextureCopies; }
            inline std::vector<TextureToBufferCopy>& GetPersistentTextureToBufferCopies() { return persistentTextureToBufferCopies; }

            inline std::vector<BufferUpload>& GetTransientBufferUploads() { return transientBufferUploads; }
            inline std::vector<TextureUpload>& GetTransientTextureUploads() { return transientTextureUploads; }

            inline std::vector<BufferUpload>& GetPersistentBufferUploads() { return persistentBufferUploads; }
            inline std::vector<TextureUpload>& GetPersistentTextureUploads() { return persistentTextureUploads; }
            
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;

            std::vector<BufferCopy> transientBufferCopies;
            std::vector<TextureCopy> transientTextureCopies;
            std::vector<BufferToTextureCopy> transientBufferToTextureCopies;
            std::vector<TextureToBufferCopy> transientTextureToBufferCopies;

            std::vector<BufferCopy> persistentBufferCopies;
            std::vector<TextureCopy> persistentTextureCopies;
            std::vector<BufferToTextureCopy> persistentBufferToTextureCopies;
            std::vector<TextureToBufferCopy> persistentTextureToBufferCopies;

            std::vector<BufferUpload> transientBufferUploads;
            std::vector<TextureUpload> transientTextureUploads;
            std::vector<BufferUpload> persistentBufferUploads;
            std::vector<TextureUpload> persistentTextureUploads;


    };

    struct ComputePass
    {
        public:
            void UseTransientTexture(TransientTextureHandle texture, Usage accessType);
            void UseTransientBuffer(TransientBufferHandle texture, Usage accessType);
            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures() { return textures; }
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers() { return buffers; }
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
    };
}