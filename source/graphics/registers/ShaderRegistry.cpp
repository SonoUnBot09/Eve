#include <graphics/registers/ShaderRegistry.hpp>

using namespace Eve::Graphics;

ShaderHandle ShaderRegistry::CreateGraphicsShader(ShaderInfo shaderInfo)
{
    GraphicsShaderObject shaderObject {};

    PipelineBuilder::BuildGraphicsPipeline(shaderInfo, shaderObject);

    shaderObjects.push_back(shaderObject);

    return ShaderHandle{.Id = static_cast<uint32_t>(shaderObjects.size() - 1)};
}