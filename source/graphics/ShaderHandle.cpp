#include <eve/graphics/ShaderHandle.hpp>
#include <graphics/registers/ShaderRegistry.hpp>

using namespace Eve::Graphics;

void ShaderHandle::SetUInt(std::string paramName, uint32_t value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void ShaderHandle::SetInt(std::string paramName, int32_t value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void ShaderHandle::SetFloat(std::string paramName, float value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 4, *this);
}
void ShaderHandle::SetVector2(std::string paramName, Vector2 value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 8, *this);
}
void ShaderHandle::SetVector2Int(std::string paramName, Vector2Int value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 8, *this);
}
void ShaderHandle::SetVector3(std::string paramName, Vector3 value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 12, *this);
}
void ShaderHandle::SetVector3Int(std::string paramName, Vector3Int value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 12, *this);
}
void ShaderHandle::SetVector4(std::string paramName, Vector4 value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 16, *this);
}
void ShaderHandle::SetVector4Int(std::string paramName, Vector4Int value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 16, *this);
}
void ShaderHandle::SetMatrix4x4(std::string paramName, Matrix4x4 value)
{
    ShaderRegistry::UpdateMaterial(paramName, &value, 64, *this);
}

uint32_t ShaderHandle::GetMaterialUBOId()
{
    return ShaderRegistry::GetMaterialUBOId(*this);
}