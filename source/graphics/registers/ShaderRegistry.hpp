#pragma once

#include "eve/graphics/ShaderHandle.hpp"
#include "graphics/builders/SlangCompiler.hpp"
#include <graphics/GraphicsCore.hpp>
#include <graphics/builders/PipelineBuilder.hpp>

namespace Eve::Graphics
{
    class ShaderRegistry
    {
        public:
            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);

            inline static GraphicsShaderObject GetShaderObject(ShaderHandle handle) { return shaderObjects[handle.Id]; }
            inline static MaterialProperties& GetMaterialProperties(ShaderHandle handle) { return materialProperties[handle.Id]; }

            static void DestroyAllShaders();
        private:
            inline static std::vector<GraphicsShaderObject> shaderObjects;
            inline static std::vector<MaterialProperties> materialProperties;
    };
}