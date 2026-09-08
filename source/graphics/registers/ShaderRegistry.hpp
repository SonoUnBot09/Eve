#pragma once

#include <graphics/GraphicsCore.hpp>
#include "eve/graphics/ShaderHandle.hpp"
#include "graphics/builders/ShaderObject.hpp"
#include "graphics/builders/SlangCompiler.hpp"
#include <graphics/builders/PipelineBuilder.hpp>

namespace Eve::Graphics
{
    class ShaderRegistry
    {
        public:

            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);
            static ComputeShaderHandle CreateComputeShader(std::string shaderModule);

            inline static GraphicsShaderObject GetShaderObject(ShaderHandle handle) { return graphicsShaderObjects[handle.Id]; }
            inline static MaterialProperties& GetMaterialProperties(ShaderHandle handle) { return materialProperties[handle.Id]; }

            inline static ComputeShaderObject GetShaderObject(ComputeShaderHandle handle) { return computeShaderObjects[handle.Id]; }

            static void DestroyAllShaders();
        private:

            inline static std::vector<GraphicsShaderObject> graphicsShaderObjects;
            inline static std::vector<MaterialProperties> materialProperties;

            inline static std::vector<ComputeShaderObject> computeShaderObjects;
    };
}