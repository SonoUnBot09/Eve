#pragma once

#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <vector>
#include <string>

#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Geometry.hpp>

namespace Eve::Graphics
{
    struct GraphicsShaderObject
    {
        VkPipelineLayout Layout;
        VkPipeline Pipeline;
        VkShaderModule VertexModule;
        VkShaderModule FragmentModule;
    };
}