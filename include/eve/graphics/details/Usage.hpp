#pragma once

namespace Eve::Graphics
{

    enum class Usage
    {
        // --- Texture Sampled ---
        VERTEX_READ_TEXTURE_SAMPLED,
        FRAGMENT_READ_TEXTURE_SAMPLED,
        VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED,

        // --- Texture Storage ---
        COMPUTE_READ_TEXTURE_STORAGE,
        COMPUTE_READ_WRITE_TEXTURE_STORAGE,

        // --- Buffers ---
        VERTEX_READ_BUFFER_STORAGE,
        VERTEX_READ_BUFFER_UNIFORM,
        FRAGMENT_READ_BUFFER_STORAGE,
        FRAGMENT_READ_BUFFER_UNIFORM,
        VERTEX_FRAGMENT_READ_BUFFER_STORAGE,
        VERTEX_FRAGMENT_READ_BUFFER_UNIFORM,
        COMPUTE_READ_BUFFER_STORAGE,
        COMPUTE_READ_BUFFER_UNIFORM,
        COMPUTE_READ_WRITE_BUFFER_STORAGE,

        // Color Depth Stencil
        COLOR_ATTACHMENT,
        DEPTH_STENCIL,
        DEPTH,
        STENCIL,
       
        // Transfer
        COPY_SOURCE,
        COPY_DESTINATION
    };

}