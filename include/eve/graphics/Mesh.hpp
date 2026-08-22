#pragma once

#include <eve/graphics/Buffer.hpp>
#include <eve/utils/Vec.hpp>

using namespace Eve::Utils;

namespace Eve::Graphics
{
    struct MeshHandle
    {
        uint32_t Id;
        uint32_t Generation;

        void SetVertices(std::vector<Vec3>& verticies);
        void SetIndicies(std::vector<uint32_t>& indicies);
        void SetNormals(std::vector<Vec3>& normals);
        void SetColors(std::vector<Vec3>& colors);
        void SetUVs(std::vector<Vec2>& uvs);
        void SetTangents(std::vector<Vec4>& tangents);

        void ApplyToGPU();
    };

    struct CPUMesh
    {
        std::vector<Vec3> Vertices;
        std::vector<uint32_t> Indicies;

        std::vector<Vec3> Normals;
        std::vector<Vec3> Colors;
        std::vector<Vec2> UVs;
        std::vector<Vec4> Tangents;
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