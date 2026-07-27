#pragma once

#include <Eve/graphics/Buffer.hpp>
#include <Eve/utils/Vec.hpp>

using namespace Eve::Utils;

namespace Eve::Graphics
{
    struct MeshHandle
    {
        uint32_t Id;
    };

    struct MeshData
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

        uint32_t VertexCount;
        uint32_t IndexCount;
    };
}