#include "MeshRegistry.hpp"
#include <eve/graphics/Buffer.hpp>
#include <eve/graphics/Pass.hpp>
#include "MemoryRegistry.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "graphics/registers/MeshRegistry.hpp"
#include <graphics/RenderGraph.hpp>
#include <graphics/DefaultMeshes.hpp>

using namespace Eve::Graphics;

MeshHandle MeshRegistry::CreateMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh;
    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = 0,
        .IndicesCount = 0,
        .NormalsCount = 0,
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = 0
    };
    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    }

    return handle;
}

MeshHandle MeshRegistry::CreateCubeMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh
    {
        .Vertices = CubeGfxMesh::positions,
        .Indices = CubeGfxMesh::indices,
        .Normals = CubeGfxMesh::normals,
        .Tangents = CubeGfxMesh::tangents
    };

    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = static_cast<uint32_t>(CubeGfxMesh::positions.size()),
        .IndicesCount = static_cast<uint32_t>(CubeGfxMesh::indices.size()),
        .NormalsCount = static_cast<uint32_t>(CubeGfxMesh::normals.size()),
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = static_cast<uint32_t>(CubeGfxMesh::tangents.size())
    };

    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    };

    ApplyMeshToGPU(handle);

    return handle;
}
MeshHandle MeshRegistry::CreateUVSphereMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh
    {
        .Vertices = UVSphereGfxMesh::positions,
        .Indices = UVSphereGfxMesh::indices,
        .Normals = UVSphereGfxMesh::normals,
        .Tangents = UVSphereGfxMesh::tangents
    };

    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = static_cast<uint32_t>(UVSphereGfxMesh::positions.size()),
        .IndicesCount = static_cast<uint32_t>(UVSphereGfxMesh::indices.size()),
        .NormalsCount = static_cast<uint32_t>(UVSphereGfxMesh::normals.size()),
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = static_cast<uint32_t>(UVSphereGfxMesh::tangents.size())
    };

    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    };

    ApplyMeshToGPU(handle);

    return handle;
}
MeshHandle MeshRegistry::CreateIcoSphereMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh
    {
        .Vertices = IcoSphereGfxMesh::positions,
        .Indices = IcoSphereGfxMesh::indices,
        .Normals = IcoSphereGfxMesh::normals,
        .Tangents = IcoSphereGfxMesh::tangents
    };

    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = static_cast<uint32_t>(IcoSphereGfxMesh::positions.size()),
        .IndicesCount = static_cast<uint32_t>(IcoSphereGfxMesh::indices.size()),
        .NormalsCount = static_cast<uint32_t>(IcoSphereGfxMesh::normals.size()),
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = static_cast<uint32_t>(IcoSphereGfxMesh::tangents.size())
    };

    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    };

    ApplyMeshToGPU(handle);

    return handle;
}
MeshHandle MeshRegistry::CreatePlaneMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh
    {
        .Vertices = PlaneGfxMesh::positions,
        .Indices = PlaneGfxMesh::indices,
        .Normals = PlaneGfxMesh::normals,
        .Tangents = PlaneGfxMesh::tangents
    };

    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = static_cast<uint32_t>(PlaneGfxMesh::positions.size()),
        .IndicesCount = static_cast<uint32_t>(PlaneGfxMesh::indices.size()),
        .NormalsCount = static_cast<uint32_t>(PlaneGfxMesh::normals.size()),
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = static_cast<uint32_t>(PlaneGfxMesh::tangents.size())
    };

    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    };

    ApplyMeshToGPU(handle);

    return handle;
}
MeshHandle MeshRegistry::CreateQuadMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh
    {
        .Vertices = QuadGfxMesh::positions,
        .Indices = QuadGfxMesh::indices,
        .Normals = QuadGfxMesh::normals,
        .Tangents = QuadGfxMesh::tangents
    };

    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = static_cast<uint32_t>(QuadGfxMesh::positions.size()),
        .IndicesCount = static_cast<uint32_t>(QuadGfxMesh::indices.size()),
        .NormalsCount = static_cast<uint32_t>(QuadGfxMesh::normals.size()),
        .ColorsCount = 0,
        .UVsCount = 0,
        .TangentsCount = static_cast<uint32_t>(QuadGfxMesh::tangents.size())
    };

    MeshBufferInfo buffersInfo
    {
        .Vertex = false,
        .Index = false,
        .Normal = false,
        .Color = false,
        .UV = false,
        .Tangent = false
    };

    if(freeSlots.empty())
    {
        uint32_t size = cpuMeshes.size();
        generations.push_back(0);

        handle.Id = size;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes.push_back(cpuMesh);
        graphicsMeshes.push_back(graphicsMesh);
        meshBuffersInfo.push_back(buffersInfo);
    }
    else 
    {
        uint32_t index = freeSlots.back();

        freeSlots.pop_back();

        handle.Id = index;
        handle.Generation = generations[handle.Id];

        generations[handle.Id]++;

        cpuMeshes[handle.Id] = cpuMesh;
        graphicsMeshes[handle.Id] = graphicsMesh;
        meshBuffersInfo[handle.Id] = buffersInfo;
    };

    ApplyMeshToGPU(handle);

    return handle;
}


void MeshRegistry::DestroyMesh(MeshHandle handle)
{
    GraphicsMesh& graphicsMesh = graphicsMeshes[handle.Id];
    MeshBufferInfo& bufferInfo = meshBuffersInfo[handle.Id];

    if(bufferInfo.Vertex)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.VertexBuffer);
    }

    if(bufferInfo.Index)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.IndexBuffer);
    }

    if(bufferInfo.Normal)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.NormalBuffer);
    }

    if(bufferInfo.Color)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.ColorBuffer);
    }

    if(bufferInfo.UV)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.UVBuffer);
    }

    if(bufferInfo.Tangent)
    {
        MemoryRegistry::DestroyBuffer(graphicsMesh.TangentBuffer);
    }

    freeSlots.push_back(handle.Id);
}

void MeshRegistry::ApplyMeshToGPU(MeshHandle handle)
{
    CPUMesh& cpuMesh = cpuMeshes[handle.Id];
    GraphicsMesh& graphicsMesh = graphicsMeshes[handle.Id];
    MeshBufferInfo& bufferInfo = meshBuffersInfo[handle.Id];

    TransferPass& universalTransferPass = RenderGraph::GetUniversalTransferPass();

    // --- Verticies ---
    if(!cpuMesh.Vertices.empty())
    {
        if(bufferInfo.Vertex)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.VertexBuffer, cpuMesh.Vertices.size() * sizeof(glm::vec3));
        }
        else 
        {
            uint64_t size = cpuMesh.Vertices.size() * sizeof(glm::vec3);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.VertexBuffer = newBuffer;

            bufferInfo.Vertex = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.Vertices.data(), graphicsMesh.VertexBuffer, cpuMesh.Vertices.size() * sizeof(glm::vec3), 0);
        graphicsMesh.VerticiesCount = cpuMesh.Vertices.size();
    }

    // --- Indices ---
    if(!cpuMesh.Indices.empty())
    {
        if(bufferInfo.Index)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.IndexBuffer, cpuMesh.Indices.size() * sizeof(uint32_t));
        }
        else 
        {
            uint64_t size = cpuMesh.Indices.size() * sizeof(uint32_t);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.IndexBuffer = newBuffer;

            bufferInfo.Index = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.Indices.data(), graphicsMesh.IndexBuffer, cpuMesh.Indices.size() * sizeof(uint32_t), 0);
        graphicsMesh.IndicesCount = cpuMesh.Indices.size();
    }

    // --- Normals ---
    if(!cpuMesh.Normals.empty())
    {
        if(bufferInfo.Normal)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.NormalBuffer, cpuMesh.Normals.size() * sizeof(glm::vec3));
        }
        else 
        {
            uint64_t size = cpuMesh.Normals.size() * sizeof(glm::vec3);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.NormalBuffer = newBuffer;

            bufferInfo.Normal = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.Normals.data(), graphicsMesh.NormalBuffer, cpuMesh.Normals.size() * sizeof(glm::vec3), 0);
        graphicsMesh.NormalsCount = cpuMesh.Normals.size();
    }

    // --- Colors ---
    if(!cpuMesh.Colors.empty())
    {
        if(bufferInfo.Color)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.ColorBuffer, cpuMesh.Colors.size() * sizeof(glm::vec3));
        }
        else 
        {
            uint64_t size = cpuMesh.Colors.size() * sizeof(glm::vec3);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.ColorBuffer = newBuffer;

            bufferInfo.Color = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.Colors.data(), graphicsMesh.ColorBuffer, cpuMesh.Colors.size() * sizeof(glm::vec3), 0);
        graphicsMesh.ColorsCount = cpuMesh.Colors.size();
    }

    // --- Uvs ---
    if(!cpuMesh.UVs.empty())
    {
        if(bufferInfo.UV)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.UVBuffer, cpuMesh.UVs.size() * sizeof(glm::vec2));
        }
        else 
        {
            uint64_t size = cpuMesh.UVs.size() * sizeof(glm::vec2);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.UVBuffer = newBuffer;

            bufferInfo.UV = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.UVs.data(), graphicsMesh.UVBuffer, cpuMesh.UVs.size() * sizeof(glm::vec2), 0);

        graphicsMesh.UVsCount = cpuMesh.UVs.size();
    }

    // --- Tangents ---
    if(!cpuMesh.Tangents.empty())
    {
        if(bufferInfo.Tangent)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.TangentBuffer, cpuMesh.Tangents.size() * sizeof(glm::vec4));
        }
        else 
        {
            uint64_t size = cpuMesh.Tangents.size() * sizeof(glm::vec4);

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(size);

            graphicsMesh.TangentBuffer = newBuffer;

            bufferInfo.Tangent = true;
        }

        universalTransferPass.UploadBuffer(cpuMesh.Tangents.data(), graphicsMesh.TangentBuffer, cpuMesh.Tangents.size() * sizeof(glm::vec4), 0);
        graphicsMesh.TangentsCount = cpuMesh.Tangents.size();
    }
}

void MeshRegistry::SetVertices(MeshHandle meshHandle, std::vector<glm::vec3>& verticies)
{
    cpuMeshes[meshHandle.Id].Vertices = verticies;
}
void MeshRegistry::SetIndices(MeshHandle meshHandle, std::vector<uint32_t>& indices)
{
    cpuMeshes[meshHandle.Id].Indices = indices;
}
void MeshRegistry::SetNormals(MeshHandle meshHandle, std::vector<glm::vec3>& normals)
{
    cpuMeshes[meshHandle.Id].Normals = normals;
}
void MeshRegistry::SetColors(MeshHandle meshHandle, std::vector<glm::vec3>& colors)
{
    cpuMeshes[meshHandle.Id].Colors = colors;
}
void MeshRegistry::SetUVs(MeshHandle meshHandle, std::vector<glm::vec2>& uvs)
{
    cpuMeshes[meshHandle.Id].UVs = uvs;
}
void MeshRegistry::SetTangents(MeshHandle meshHandle, std::vector<glm::vec4>& tangents)
{
    cpuMeshes[meshHandle.Id].Tangents = tangents;
}