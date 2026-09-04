#include <eve/graphics/MaterialHandle.hpp>
#include <graphics/registers/MaterialRegistry.hpp>

using namespace Eve::Graphics;

void MaterialHandle::SetUInt(std::string paramName, uint32_t value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void MaterialHandle::SetInt(std::string paramName, int32_t value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void MaterialHandle::SetFloat(std::string paramName, float value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void MaterialHandle::SetVector2(std::string paramName, glm::vec2 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 8, *this);
}
void MaterialHandle::SetVector2Int(std::string paramName, glm::ivec2 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 8, *this);
}
void MaterialHandle::SetVector3(std::string paramName, glm::vec3 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 12, *this);
}
void MaterialHandle::SetVector3Int(std::string paramName, glm::ivec3 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 12, *this);
}
void MaterialHandle::SetVector4(std::string paramName, glm::vec4 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 16, *this);
}
void MaterialHandle::SetVector4Int(std::string paramName, glm::ivec4 value)
{
    MaterialRegistry::UpdateMaterial(paramName, &value, 16, *this);
}

uint32_t MaterialHandle::GetPropertiesUBOId()
{
    return MaterialRegistry::GetPropertiesUBOId(*this);
}

ShaderHandle MaterialHandle::GetShader()
{
    return MaterialRegistry::GetShader(*this);
}

void MaterialHandle::SetShader(ShaderHandle handle)
{
    MaterialRegistry::SetShader(*this, handle);
}