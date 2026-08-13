#pragma once

#include <graphics/builders/PipelineBuilder.hpp>

namespace Eve::Graphics
{
    class ShaderRegistry
    {
        public:
            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);
            inline static GraphicsShaderObject GetShaderObject(ShaderHandle handle) { return shaderObjects[handle.Id]; }

            static void DestroyAllShaders();
        private:
            inline static std::vector<GraphicsShaderObject> shaderObjects;
    };
}