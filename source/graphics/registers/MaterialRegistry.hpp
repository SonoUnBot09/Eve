#pragma once

#include <eve/graphics/ShaderHandle.hpp>
#include <eve/graphics/MaterialHandle.hpp>
#include <vector>
#include <eve/graphics/Buffer.hpp>
#include <array>
#include <graphics/GraphicsCore.hpp>
#include <iostream>

namespace Eve::Graphics 
{
    struct MaterialObject
    {
        std::vector<BufferHandle> UBOs;
        std::array<std::byte, 16 * 1024> MaterialData;
        ShaderHandle Shader;
        uint32_t Countdown;
    };

    class MaterialRegistry
    {
        public:
            static MaterialHandle CreateMaterial(ShaderHandle shader);

            inline static void SetShader(MaterialHandle handle, ShaderHandle shader) { materials[handle.Id].Shader = shader; }

            inline static ShaderHandle GetShader(MaterialHandle handle) { return materials[handle.Id].Shader; }
            inline static uint32_t GetPropertiesUBOId(MaterialHandle handle) { return materials[handle.Id].UBOs[GraphicsCore::GetFrameIndex()].Id; }

            static void UpdateMaterial(std::string& paramName, void* value, uint32_t paramSize, MaterialHandle handle);

            static void UploadMaterials();
        private:
            inline static std::vector<MaterialObject> materials;
    };
}