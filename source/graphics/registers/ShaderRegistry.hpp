#pragma once

#include "eve/graphics/ShaderHandle.hpp"
#include <graphics/GraphicsCore.hpp>
#include <graphics/builders/PipelineBuilder.hpp>

namespace Eve::Graphics
{
    struct PropertiesUBOs
    {
        MaterialProperties Properties;
        std::vector<BufferHandle> UBOs; 
        std::array<std::byte, 16 * 1024> MaterialData;
        uint32_t countdown = 0;
    };

    class ShaderRegistry
    {
        public:
            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);
            static void UpdateMaterial(std::string& paramName, void* value, uint32_t paramSize, ShaderHandle handle);

            inline static GraphicsShaderObject GetShaderObject(ShaderHandle handle) { return shaderObjects[handle.Id]; }
            inline static uint32_t GetMaterialUBOId(ShaderHandle handle) { return properties[handle.Id].UBOs[GraphicsCore::GetFrameIndex()].Id; }

            static void UploadMaterials();

            static void DestroyAllShaders();
        private:
            inline static std::vector<GraphicsShaderObject> shaderObjects;
            inline static std::vector<PropertiesUBOs> properties;
    };
}