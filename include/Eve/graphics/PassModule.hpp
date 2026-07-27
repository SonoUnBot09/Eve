#pragma once

#include <vector>
#include <utility>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Buffer.hpp>
#include <Eve/graphics/Mesh.hpp>

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

    struct GraphicsPass
    {
        public:
            void UseTransientTexture(TransientTextureHandle texture, Usage accessType);
            void UseTransientBuffer(TransientBufferHandle buffer, Usage accessType);
            void DrawMesh(MeshHandle mesh, uint32_t instanceCount);
            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures();
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers();
            std::vector<std::pair<MeshHandle, uint32_t>>& GetDrawCalls();
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
            std::vector<std::pair<MeshHandle, uint32_t>> drawCalls;
    };

    struct TransferPass
    {
        public:
            void UseTransientTexture(Usage accessType);
            void UseTransientBuffer(Usage accessType);
            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures();
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers();
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
    };

    struct ComputePass
    {
        public:
            void UseTransientTexture(Usage accessType);
            void UseTransientBuffer(Usage accessType);
            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures();
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers();
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
    };
}