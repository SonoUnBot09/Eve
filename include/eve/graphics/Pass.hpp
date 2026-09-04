#pragma once

#include "eve/graphics/MaterialHandle.hpp"
#include <vector>
#include <utility>
#include <eve/graphics/Texture.hpp>
#include <eve/graphics/Buffer.hpp>
#include <eve/graphics/Mesh.hpp>
#include <eve/graphics/ShaderHandle.hpp>
#include <eve/graphics/details/Usage.hpp>
#include <eve/components/Transform.hpp>
#include <eve/graphics/RenderViewHandle.hpp>

namespace Eve::Graphics
{
    class RenderGraph;
    class MeshRegistry;

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

    struct DrawInfo
    {
        explicit DrawInfo (void* data, uint32_t sizeBytes) :  Data(data), SizeBytes(sizeBytes) {};
        void* Data;
        uint32_t SizeBytes;
    };

    struct DrawCall
    {
        // Shader
        uint32_t VertexShaderInvocations;
        uint32_t InstanceCount;

        // Material
        MaterialHandle MaterialHandle;

        // Draw Info Params
        std::vector<std::byte> DrawInfoData;

        // Render View
        RenderViewHandle RenderView;
    };

    struct InstanceParams
    {
        glm::mat4 ObjectToWorld;
        glm::mat4 WorldToObject;
    };

    struct LoadStoreOp
    {
        LoadOperation loadOp;
        StoreOperation storeOp;
        glm::vec3 clearColor;
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
        glm::ivec3 Extent;
        glm::ivec3 SrcOffset;
        glm::ivec3 DstOffset;
    };

    struct BufferToTextureCopy
    {
        uint32_t SrcBuffer; 
        uint32_t DstTexture; 
        uint64_t SrcOffset;
        uint32_t BufferRowLength;
        uint32_t BufferHeightLength;
        glm::ivec3 DstOffset;
        glm::ivec3 Extent;
    };

    struct TextureToBufferCopy
    {
        uint32_t SrcTexture; 
        uint32_t DstBuffer; 
        glm::ivec3 SrcOffset;
        glm::ivec3 Extent;
        uint64_t DstOffset;
        uint32_t BufferRowLength;
        uint32_t BufferHeightLength;
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
        glm::ivec3 DstOffset;
        glm::ivec3 Extent;
        uint32_t BufferRowLength;
        uint32_t BufferHeightLength;
    };

    struct GraphicsPass
    {
        public:

            void UseTransientTexture(TransientTextureHandle texture, Usage accessType);
            void UseTransientBuffer(TransientBufferHandle buffer, Usage accessType);

            void UseTextureVertex(TransientTextureHandle texture);
            void UseTextureFragment(TransientTextureHandle texture);
            void UseTextureVertexFragment(TransientTextureHandle texture);

            void UseTextureVertex(TextureHandle texture);
            void UseTextureFragment(TextureHandle texture);
            void UseTextureVertexFragment(TextureHandle texture);

            void UseBufferReadOnlyVertex(BufferHandle buffer);
            void UseBufferReadOnlyFragment(BufferHandle buffer);
            void UseBufferReadOnlyVertexFragment(BufferHandle buffer);

            void UseBufferReadOnlyVertex(TransientBufferHandle buffer);
            void UseBufferReadOnlyFragment(TransientBufferHandle buffer);
            void UseBufferReadOnlyVertexFragment(TransientBufferHandle buffer);

            void UseColorTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseDepthStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseDepthTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);
            void UseStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp);

            void Draw(uint32_t vertexShaderInvocations, Transform& transform, MaterialHandle material, RenderViewHandle renderView, DrawInfo* drawInfo);
            void DrawInstanced(uint32_t vertexShaderInvocations, uint32_t instanceCount, Transform& transforms, MaterialHandle material, RenderViewHandle renderView,  DrawInfo* drawInfo);

        private:

            inline std::vector<std::pair<TransientTextureHandle, Usage>>& GetTransientTextures() { return transientTextures; }
            inline std::vector<std::pair<TransientBufferHandle, Usage>>& GetTransientBuffers() { return transientBuffers; }
            inline std::vector<std::pair<TextureHandle, Usage>>& GetPersistentTextures() { return persistentTextures; }
            inline std::vector<std::pair<BufferHandle, Usage>>& GetPersistentBuffers() { return persistentBuffers; }

            inline std::vector<std::pair<TransientTextureHandle, LoadStoreOp>>& GetLoadStoreOperations() { return loadStoreOps; }

            inline std::vector<DrawCall>& GetDrawCalls() { return drawCalls; }
            inline std::vector<InstanceParams>& GetInstanceParams() { return instanceParams; }

            void Clear();

            std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;
            std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;

            std::vector<std::pair<TextureHandle, Usage>> persistentTextures;
            std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;

            std::vector<std::pair<TransientTextureHandle, LoadStoreOp>> loadStoreOps;

            std::vector<DrawCall> drawCalls;
            std::vector<InstanceParams> instanceParams;

            friend class RenderGraph;
    };

    struct TransferPass
    {
        public:

            // Transient
            void CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);
            void CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0});
            void CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);
            void CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);

            // Persistent
            void CopyBuffer(BufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);
            void CopyTexture(TextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0});
            void CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);
            void CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLenght = 0);

            // Buffer Mix
            void CopyBuffer(TransientBufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);
            void CopyBuffer(BufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0);

            // Texture Mix
            void CopyTexture(TransientTextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0});
            void CopyTexture(TextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0});

            // Buffer To Texture Mix
            void CopyBufferToTexture(TransientBufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);
                void CopyBufferToTexture(BufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);

            // Texture To Buffer Mix
            void CopyTextureToBuffer(TransientTextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);
            void CopyTextureToBuffer(TextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, 
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);

            // Transient
            void UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TransientTextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);

            // Persistent
            void UploadBuffer(void* SrcData, BufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0);
            void UploadTexture(void* SrcData, uint64_t Size, TextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent,
                uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0);
            
        private:

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

            inline std::vector<BufferCopy>& GetTransientPersistentBufferCopies() { return transientPersistentBufferCopies; }
            inline std::vector<BufferCopy>& GetPersistentTransientBufferCopies() { return persistentTransientBufferCopies; }
            inline std::vector<TextureCopy>& GetTransientPersistentTextureCopies() { return transientPersistentTextureCopies; }
            inline std::vector<TextureCopy>& GetPersistentTransientTextureCopies() { return persistentTransientTextureCopies; }

            inline std::vector<BufferToTextureCopy>& GetTransientPersistentBufferToTextureCopies() { return transientPersistentBufferToTextureCopies; }
            inline std::vector<BufferToTextureCopy>& GetPersistentTransientBufferToTextureCopies() { return persistentTransientBufferToTextureCopies; }
            inline std::vector<TextureToBufferCopy>& GetTransientPersistentTextureToBufferCopies() { return transientPersistentTextureToBufferCopies; }
            inline std::vector<TextureToBufferCopy>& GetPersistentTransientTextureToBufferCopies() { return persistentTransientTextureToBufferCopies; }

            inline std::vector<BufferUpload>& GetTransientBufferUploads() { return transientBufferUploads; }
            inline std::vector<TextureUpload>& GetTransientTextureUploads() { return transientTextureUploads; }

            inline std::vector<BufferUpload>& GetPersistentBufferUploads() { return persistentBufferUploads; }
            inline std::vector<TextureUpload>& GetPersistentTextureUploads() { return persistentTextureUploads; }

            void Clear();

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

            friend class RenderGraph;
            friend class MeshRegistry;

    };

    struct ComputePass
    {
        public:
            void UseTransientTexture(TransientTextureHandle texture, Usage accessType);
            void UseTransientBuffer(TransientBufferHandle texture, Usage accessType);

        private:

            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTransientTextures() { return transientTextures; }
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetTransientBuffers() { return transientBuffers; }
            std::vector<std::pair<TextureHandle, Usage>>& GetPersistentTextures() { return persistentTextures; }
            std::vector<std::pair<BufferHandle, Usage>>& GetPersistentBuffers() { return persistentBuffers; }

            void Clear();

            std::vector<std::pair<TransientTextureHandle, Usage>> transientTextures;
            std::vector<std::pair<TransientBufferHandle, Usage>> transientBuffers;
            std::vector<std::pair<TextureHandle, Usage>> persistentTextures;
            std::vector<std::pair<BufferHandle, Usage>> persistentBuffers;

            friend class RenderGraph;
    };
}