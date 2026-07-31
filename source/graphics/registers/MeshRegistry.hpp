#pragma once

#include <Eve/graphics/Mesh.hpp>

namespace Eve::Graphics
{
    class MeshRegistry
    {
        public:
            inline static void CreateMesh(std::vector<Vec3> vertices, std::vector<uint32_t> indicies);
            inline static CPUMesh& GetMeshData(MeshHandle handle) { return meshData[handle.Id]; }
            inline static GraphicsMesh& GetGraphicsMesh(MeshHandle handle) { return graphicsMeshes[handle.Id]; } 
        private:
            static inline std::vector<CPUMesh> meshData;
            static inline std::vector<GraphicsMesh> graphicsMeshes;
    };
}