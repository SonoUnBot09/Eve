#include <eve/graphics/Mesh.hpp>
#include <graphics/registers/MeshRegistry.hpp>

using namespace Eve::Graphics;

void MeshHandle::SetVertices(std::vector<Vec3>& verticies)
{
    MeshRegistry::SetVertices(*this, verticies);
}
void MeshHandle::SetIndicies(std::vector<uint32_t>& indicies)
{
    MeshRegistry::SetIndicies(*this,indicies);
}
void MeshHandle::SetNormals(std::vector<Vec3>& normals)
{
    MeshRegistry::SetNormals(*this, normals);
}
void MeshHandle::SetColors(std::vector<Vec3>& colors)
{
    MeshRegistry::SetColors(*this, colors);
}
void MeshHandle::SetUVs(std::vector<Vec2>& uvs)
{
    MeshRegistry::SetUVs(*this, uvs);
}
void MeshHandle::SetTangents(std::vector<Vec4>& tangents)
{
    MeshRegistry::SetTangents(*this, tangents);
}
void MeshHandle::ApplyToGPU()
{
    MeshRegistry::ApplyMeshToGPU(*this);
}