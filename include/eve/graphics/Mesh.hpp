#pragma once

#include <eve/graphics/Buffer.hpp>
#include <vector>
#include <eve/math/Vector2.hpp>
#include <eve/math/Vector3.hpp>
#include <eve/math/Vector4.hpp>

using namespace Eve::Math;

namespace Eve::Graphics
{
    struct MeshHandle
    {
        uint32_t Id;
        uint32_t Generation;

        void SetVertices(std::vector<Vector3>& verticies);
        void SetIndicies(std::vector<uint32_t>& indicies);
        void SetNormals(std::vector<Vector3>& normals);
        void SetColors(std::vector<Vector3>& colors);
        void SetUVs(std::vector<Vector2>& uvs);
        void SetTangents(std::vector<Vector4>& tangents);

        void ApplyToGPU();
    };

    struct CPUMesh
    {
        std::vector<Vector3> Vertices;
        std::vector<uint32_t> Indicies;

        std::vector<Vector3> Normals;
        std::vector<Vector3> Colors;
        std::vector<Vector2> UVs;
        std::vector<Vector4> Tangents;
    };

    struct GraphicsMesh
    {
        BufferHandle VertexBuffer;
        BufferHandle IndexBuffer;
        
        BufferHandle NormalBuffer;
        BufferHandle ColorBuffer;
        BufferHandle UVBuffer;
        BufferHandle TangentBuffer;

        uint32_t VerticiesCount;
        uint32_t IndiciesCount;
        uint32_t NormalsCount;
        uint32_t ColorsCount;
        uint32_t UVsCount;
        uint32_t TangentsCount;
    };
}