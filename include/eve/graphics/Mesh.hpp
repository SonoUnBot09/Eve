#pragma once

#include <eve/graphics/Buffer.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace Eve::Graphics
{
    struct MeshHandle
    {
        uint32_t Id;
        uint32_t Generation;

        void SetVertices(std::vector<glm::vec3>& verticies);
        void SetIndicies(std::vector<uint32_t>& indicies);
        void SetNormals(std::vector<glm::vec3>& normals);
        void SetColors(std::vector<glm::vec3>& colors);
        void SetUVs(std::vector<glm::vec2>& uvs);
        void SetTangents(std::vector<glm::vec4>& tangents);

        void ApplyToGPU();
    };

    struct CPUMesh
    {
        std::vector<glm::vec3> Vertices;
        std::vector<uint32_t> Indicies;

        std::vector<glm::vec3> Normals;
        std::vector<glm::vec3> Colors;
        std::vector<glm::vec2> UVs;
        std::vector<glm::vec4> Tangents;
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