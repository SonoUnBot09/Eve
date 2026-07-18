#pragma once

#include <vector>
#include <utility>
#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{

    enum class Usage
    {
        NONE,
        
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
            void UseTransientTexture(Usage accessType);
            void UseTransientBuffer(Usage accessType);
            std::vector<std::pair<TransientTextureHandle, Usage>>& GetTextures();
            std::vector<std::pair<TransientBufferHandle, Usage>>& GetBuffers();
        private:
            std::vector<std::pair<TransientTextureHandle, Usage>> textures;
            std::vector<std::pair<TransientBufferHandle, Usage>> buffers;
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