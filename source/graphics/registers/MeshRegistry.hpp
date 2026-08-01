#pragma once

#include <Eve/graphics/Mesh.hpp>
#include <Eve/graphics/PassModule.hpp>

namespace Eve::Graphics
{
    class MeshRegistry
    {
        public:

            inline static void CreateMesh(CPUMesh& mesh);
            inline static CPUMesh& GetMeshData(MeshHandle handle) { return meshData[handle.Id]; }
            inline static GraphicsMesh& GetGraphicsMesh(MeshHandle handle) { return graphicsMeshes[handle.Id]; } 

        private:

            static inline std::vector<CPUMesh> meshData;
            static inline std::vector<GraphicsMesh> graphicsMeshes;

            TransferPass transferPass;

    };
}