#pragma once

#include "eve/graphics/ShaderHandle.hpp"
#include <graphics/builders/PipelineBuilder.hpp>

namespace Eve::Graphics
{
    struct PropertiesUBOs
    {
        MaterialProperties Properties;
        std::vector<BufferHandle> UBOs; 
        std::array<std::byte, 16 * 1024> MaterialData;
        bool IsDirty = false;
    };

    class ShaderRegistry
    {
        public:
            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);
            static void UpdateMaterial(std::string& paramName, void* value, uint32_t paramSize, ShaderHandle handle);
            inline static GraphicsShaderObject GetShaderObject(ShaderHandle handle) { return shaderObjects[handle.Id]; }

            static void DestroyAllShaders();
        private:
            inline static std::vector<GraphicsShaderObject> shaderObjects;
            inline static std::vector<PropertiesUBOs> properties;
    };
}