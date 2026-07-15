#pragma once

#include <cstdint>

#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    struct CommandBufferHandle
    {
        uint32_t Id;

        void MapImage(TextureHandle handle);
        void MapSampler(SamplerHandle handle);
        void MapBuffer(BufferHandle buffer);
    };

    
}