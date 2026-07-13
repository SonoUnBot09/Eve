#pragma once

#include <cstdint>

#include <Eve/graphics/Image.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    struct CommandBufferHandle
    {
        uint32_t Id;

        void MapImage(ImageHandle handle);
        void MapSampler(SamplerHandle handle);
        void MapBuffer(BufferHandle buffer);
    };

    
}