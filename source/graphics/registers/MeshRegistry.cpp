#include "MeshRegistry.hpp"
#include "Eve/graphics/Buffer.hpp"
#include "Eve/graphics/PassModule.hpp"
#include "Eve/utils/Vec.hpp"
#include "MemoryRegistry.hpp"
#include <graphics/RenderGraph.hpp>

using namespace Eve::Graphics;

MeshHandle MeshRegistry::CreateMesh()
{
    MeshHandle handle;

    CPUMesh cpuMesh;
    GraphicsMesh graphicsMesh
    {
        .VerticiesCount = 0,
        .IndiciesCount = 0,
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

    // --- Verticies ---
    if(!cpuMesh.Vertices.empty())
    {
        if(bufferInfo.Vertex)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.VertexBuffer, cpuMesh.Vertices.size() * sizeof(Vec3), false);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.Vertices.size() * sizeof(Vec3),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.VertexBuffer = newBuffer;

            bufferInfo.Vertex = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.Vertices.data(), graphicsMesh.VertexBuffer, cpuMesh.Vertices.size() * sizeof(Vec3), 0);
        graphicsMesh.VerticiesCount = cpuMesh.Vertices.size();
    }

    // --- Indicies ---
    if(!cpuMesh.Indicies.empty())
    {
        if(bufferInfo.Index)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.IndexBuffer, cpuMesh.Indicies.size() * sizeof(uint32_t), true);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.Indicies.size() * sizeof(uint32_t),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | 
                    BufferUsage::BUFFER_USAGE_TRANSFER_DST | BufferUsage::BUFFER_USAGE_INDEX
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.IndexBuffer = newBuffer;

            bufferInfo.Index = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.Indicies.data(), graphicsMesh.IndexBuffer, cpuMesh.Indicies.size() * sizeof(Vec3), 0);
        graphicsMesh.IndiciesCount = cpuMesh.Indicies.size();
    }

    // --- Normals ---
    if(!cpuMesh.Normals.empty())
    {
        if(bufferInfo.Normal)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.NormalBuffer, cpuMesh.Normals.size() * sizeof(Vec3), false);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.Normals.size() * sizeof(Vec3),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.NormalBuffer = newBuffer;

            bufferInfo.Normal = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.Normals.data(), graphicsMesh.NormalBuffer, cpuMesh.Normals.size() * sizeof(Vec3), 0);
        graphicsMesh.NormalsCount = cpuMesh.Normals.size();
    }

    // --- Colors ---
    if(!cpuMesh.Colors.empty())
    {
        if(bufferInfo.Color)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.ColorBuffer, cpuMesh.Colors.size() * sizeof(Vec3), false);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.Colors.size() * sizeof(Vec3),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.ColorBuffer = newBuffer;

            bufferInfo.Color = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.Colors.data(), graphicsMesh.ColorBuffer, cpuMesh.Colors.size() * sizeof(Vec3), 0);
        graphicsMesh.ColorsCount = cpuMesh.Colors.size();
    }

    // --- Uvs ---
    if(!cpuMesh.UVs.empty())
    {
        if(bufferInfo.UV)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.UVBuffer, cpuMesh.UVs.size() * sizeof(Vec2), false);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.UVs.size() * sizeof(Vec2),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.UVBuffer = newBuffer;

            bufferInfo.UV = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.UVs.data(), graphicsMesh.UVBuffer, cpuMesh.UVs.size() * sizeof(Vec2), 0);

        graphicsMesh.UVsCount = cpuMesh.UVs.size();
    }

    // --- Tangents ---
    if(!cpuMesh.Tangents.empty())
    {
        if(bufferInfo.Tangent)
        {
            MemoryRegistry::ResizeBufferIfNeeded(graphicsMesh.TangentBuffer, cpuMesh.Tangents.size() * sizeof(Vec4), false);
        }
        else 
        {
            BufferInfo info
            {
                .Size = cpuMesh.Tangents.size() * sizeof(Vec4),
                .Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_SRC | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle newBuffer = MemoryRegistry::CreateGPUBuffer(info);

            graphicsMesh.TangentBuffer = newBuffer;

            bufferInfo.Tangent = true;
        }

        MeshRegistry::transferPass.UploadBuffer(cpuMesh.Tangents.data(), graphicsMesh.TangentBuffer, cpuMesh.Tangents.size() * sizeof(Vec4), 0);
        graphicsMesh.TangentsCount = cpuMesh.Tangents.size();
    }
}

void MeshRegistry::SetVertices(MeshHandle meshHandle, std::vector<Vec3>& verticies)
{
    cpuMeshes[meshHandle.Id].Vertices = verticies;
}
void MeshRegistry::SetIndicies(MeshHandle meshHandle, std::vector<uint32_t>& indicies)
{
    cpuMeshes[meshHandle.Id].Indicies = indicies;
}
void MeshRegistry::SetNormals(MeshHandle meshHandle, std::vector<Vec3>& normals)
{
    cpuMeshes[meshHandle.Id].Normals = normals;
}
void MeshRegistry::SetColors(MeshHandle meshHandle, std::vector<Vec3>& colors)
{
    cpuMeshes[meshHandle.Id].Colors = colors;
}
void MeshRegistry::SetUVs(MeshHandle meshHandle, std::vector<Vec2>& uvs)
{
    cpuMeshes[meshHandle.Id].UVs = uvs;
}
void MeshRegistry::SetTangents(MeshHandle meshHandle, std::vector<Vec4>& tangents)
{
    cpuMeshes[meshHandle.Id].Tangents = tangents;
}

void MeshRegistry::UploadMeshes()
{
    RenderGraph::AddPass(transferPass, 0);
    transferPass.GetPersistentBufferUploads().clear();
}