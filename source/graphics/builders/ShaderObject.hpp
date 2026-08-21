#pragma once

#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <vector>
#include <string>

#include <eve/graphics/Texture.hpp>
#include <eve/graphics/Geometry.hpp>

namespace Eve::Graphics
{
    struct GraphicsShaderObject
    {
        VkPipeline Pipeline;
        VkShaderModule VertexModule;
        VkShaderModule FragmentModule;
    };
}