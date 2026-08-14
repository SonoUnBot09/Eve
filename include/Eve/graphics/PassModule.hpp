#pragma once

#include "Eve/utils/Vec.hpp"
#include <array>
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
        // --- Texture Sampled ---
        VERTEX_READ_TEXTURE_SAMPLED,
        FRAGMENT_READ_TEXTURE_SAMPLED,
        VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED,

        // --- Texture Storage ---
        VERTEX_READ_TEXTURE_STORAGE,
        FRAGMENT_READ_TEXTURE_STORAGE,
        VERTEX_FRAGMENT_READ_TEXTURE_STORAGE,
        COMPUTE_READ_TEXTURE_STORAGE,
        COMPUTE_WRITE_TEXTURE_STORAGE,

        // --- Buffers ---
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
        DEPTH_STENCIL,
        DEPTH,
        STENCIL,
       
        // Transfer
        COPY_SOURCE,
        COPY_DESTINATION
    };

    enum class LoadOperation
    {
        LOAD,
        DISCARD,
        CLEAR
    };

    enum class StoreOperation
    {
        STORE,
        DISCARD
    };

    struct DrawCall
    {
        MeshHandle MeshHandle;
        ShaderHandle ShaderHandle;
        uint32_t InstanceCount;
        std::array<std::byte, 128> PushCostant;
        uint32_t Offset;
        uint32_t Size;
    };

    struct LoadStoreOp
    {
        LoadOperation loadOp;
        StoreOperation storeOp;
        Vec3 clearColor;
        float clearDepth;
        uint8_t clearStencil;
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

            void UseColorTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseDepthStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseDepthTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);

            void DrawMesh(MeshHandle mesh, ShaderHandle shader, const void* pushConstant, uint32_t offset, uint32_t size);

            inline std::vector<std::pair<TransientTextureHandle, Usage>>& GetTransientTextures() { return transientTextures; }
            inline std::vector<std::pair<TransientBufferHandle, Usage>>& GetTransientBuffers() { return transientBuffers; }
            inline std::vector<std::pair<TextureHandle, Usage>>& GetPersistentTextures() { return persistentTextures; }
            inline std::vector<std::pair<BufferHandle, Usage>>& GetPersistentBuffers() { return persistentBuffers; }

            inline std::vector<std::pair<TransientTextureHandle, LoadStoreOp>>& GetLoadStoreOperations() { return loadStoreOps; }

            inline std::vector<DrawCall>& GetDrawCalls() { return drawCalls; }

        private:

            std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;
            std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;

            std::vector<std::pair<TextureHandle, Usage>> persistentTextures;
            std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;

            std::vector<std::pair<TransientTextureHandle, LoadStoreOp>> loadStoreOps;

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
            void CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);
            void CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, Vec3Int SrcOffset, Vec3Int Extent, uint64_t DstOffset, 
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);

            // Transient
            void UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TransientTextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);

            // Persistent
            void UploadBuffer(void* SrcData, BufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TextureHandle DstTexture, Vec3Int DstOffset, Vec3Int Extent,
                uint32_t BufferRowLenght = 0, uint32_t BufferHeightLenght = 0);
            
            inline std::vector<std::pair<TransientTextureHandle, Usage>>& GetTransientTextures() { return transientTextures; }
            inline std::vector<std::pair<TransientBufferHandle, Usage>>& GetTransientBuffers() { return transientBuffers; }
            inline std::vector<std::pair<TextureHandle, Usage>>& GetPersistentTextures() { return persistentTextures; }
            inline std::vector<std::pair<BufferHandle, Usage>>& GetPersistentBuffers() { return persistentBuffers; }


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

            std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;
            std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;
            std::vector<std::pair<TextureHandle, Usage>> persistentTextures;
            std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;

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

            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTransientTextures() { return transientTextures; }
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetTransientBuffers() { return transientBuffers; }
            std::vector<std::pair<TextureHandle, Usage>>& GetPersistentTextures() { return persistentTextures; }
            std::vector<std::pair<BufferHandle, Usage>>& GetPersistentBuffers() { return persistentBuffers; }
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;
            std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;
            std::vector<std::pair<TextureHandle, Usage>> persistentTextures;
            std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;
    };
}