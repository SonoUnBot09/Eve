#pragma once

#include "glm/ext/vector_float2.hpp"
#include <eve/graphics/Mesh.hpp>
#include <eve/graphics/Pass.hpp>

namespace Eve::Graphics
{
    class MeshRegistry
    {
        public:

            static MeshHandle CreateMesh();
            static void DestroyMesh(MeshHandle handle);
            inline static CPUMesh& GetCPUMesh(MeshHandle handle) { return cpuMeshes[handle.Id]; }
            inline static GraphicsMesh& GetGraphicsMesh(MeshHandle handle) { return graphicsMeshes[handle.Id]; } 

            static void SetVertices(MeshHandle meshHandle, std::vector<glm::vec3>& verticies);
            static void SetIndicies(MeshHandle meshHandle, std::vector<uint32_t>& indicies);
            static void SetNormals(MeshHandle meshHandle, std::vector<glm::vec3>& normals);
            static void SetColors(MeshHandle meshHandle, std::vector<glm::vec3>& colors);
            static void SetUVs(MeshHandle meshHandle, std::vector<glm::vec2>& uvs);
            static void SetTangents(MeshHandle meshHandle, std::vector<glm::vec4>& tangents);

            static void ApplyMeshToGPU(MeshHandle meshHandle);
        private:

            struct MeshBufferInfo
            {
                bool Vertex;
                bool Index;
                bool Normal;
                bool Color;
                bool UV;
                bool Tangent;
            };

            static inline std::vector<CPUMesh> cpuMeshes;                   static inline std::vector<uint32_t> generations;
            static inline std::vector<GraphicsMesh> graphicsMeshes;         static inline std::vector<MeshBufferInfo> meshBuffersInfo; 

            static inline std::vector<uint32_t> freeSlots;

    };
}