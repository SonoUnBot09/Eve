#include "GraphicsCore.hpp"
#include "registers/MaterialRegistry.hpp"
#include "registers/RenderViewRegistry.hpp"
#include <eve/graphics/Graphics.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
#include <graphics/registers/MeshRegistry.hpp>
#include <graphics/registers/ShaderRegistry.hpp>
#include <graphics/RenderGraph.hpp>
#include <graphics/GraphicsCore.hpp>

using namespace Eve::Graphics;

TextureHandle Graphics::CreateTexture1D(TextureInfo1D textureInfo)
{
    return MemoryRegistry::CreateTexture1D(textureInfo);
}
TextureHandle Graphics::CreateTexture2D(TextureInfo2D textureInfo)
{
    return MemoryRegistry::CreateTexture2D(textureInfo);
}
TextureHandle Graphics::CreateTexture3D(TextureInfo3D textureInfo)
{
    return MemoryRegistry::CreateTexture3D(textureInfo);
}
TextureHandle Graphics::CreateTextureCube(TextureInfo2D textureInfo)
{
    return MemoryRegistry::CreateTextureCube(textureInfo);
}

// --- Sampler Creation ---
SamplerHandle Graphics::CreateSampler(SamplerInfo samplerInfo)
{
    return MemoryRegistry::CreateSampler(samplerInfo);
}

// --- Buffer Creation ---
BufferHandle Graphics::CreateGPUBuffer(uint64_t size)
{
    return MemoryRegistry::CreateGPUBuffer(size);
}

// --- Resource Destruction ---
void Graphics::DestroyBuffer(BufferHandle handle)
{
    MemoryRegistry::DestroyBuffer(handle);
}
void Graphics::DestroyTexture(TextureHandle handle)
{
    MemoryRegistry::DestroyTexture(handle);
}
void Graphics::DestroySampler(SamplerHandle handle)
{
    MemoryRegistry::DestroySampler(handle);
}

// --- Meshes ---
MeshHandle Graphics::CreateMesh()
{
    return MeshRegistry::CreateMesh();
}
void Graphics::DestroyMesh(MeshHandle handle)
{
    MeshRegistry::DestroyMesh(handle);
}
CPUMesh& Graphics::GetCPUMesh(MeshHandle handle)
{
    return MeshRegistry::GetCPUMesh(handle);
}
GraphicsMesh& Graphics::GetGraphicsMesh(MeshHandle handle)
{
    return MeshRegistry::GetGraphicsMesh(handle);
}

// --- Shaders & Materials ---
ShaderHandle Graphics::CreateGraphicsShader(ShaderInfo shaderInfo)
{
    return ShaderRegistry::CreateGraphicsShader(shaderInfo);
}

RenderViewHandle Graphics::CreateRenderView()
{
    return RenderViewRegistry::CreateRenderView();
}
void Graphics::DestroyRenderView(RenderViewHandle handle)
{
    RenderViewRegistry::DestroyRenderView(handle);
}

MaterialHandle Graphics::CreateMaterial(ShaderHandle handle)
{
    return MaterialRegistry::CreateMaterial(handle);
}

TransientTextureHandle Graphics::RequestTransientTexture1D(TransientTextureInfo1D textureInfo)
{
    return RenderGraph::RequestTransientTexture1D(textureInfo);
}
TransientTextureHandle Graphics::RequestTransientTexture2D(TransientTextureInfo2D textureInfo)
{
    return RenderGraph::RequestTransientTexture2D(textureInfo);
}
TransientTextureHandle Graphics::RequestTransientTexture3D(TransientTextureInfo3D textureInfo)
{
    return RenderGraph::RequestTransientTexture3D(textureInfo);
}
TransientTextureHandle Graphics::RequestTransientTextureCube(TransientTextureInfo2D textureInfo)
{
    return RenderGraph::RequestTransientTextureCube(textureInfo);
}
TransientBufferHandle Graphics::RequestTransientBuffer(uint64_t size)
{
    return RenderGraph::RequestTransientBuffer(size);
} 

void Graphics::AddPass(GraphicsPass& pass)
{
    RenderGraph::AddPass(pass);
}
void Graphics::AddPass(TransferPass& pass)
{
    RenderGraph::AddPass(pass);
}
void Graphics::AddPass(ComputePass& pass)
{
    RenderGraph::AddPass(pass);
}
void Graphics::AddPass(GraphicsPass& pass, uint32_t index)
{
    RenderGraph::AddPass(pass, index);
}
void Graphics::AddPass(TransferPass& pass, uint32_t index)
{
    RenderGraph::AddPass(pass, index);
}
void Graphics::AddPass(ComputePass& pass, uint32_t index)
{
    RenderGraph::AddPass(pass, index);
}

void Graphics::SetPresentTexture2D(TransientTextureHandle handle)
{
    RenderGraph::SetPresentTexture2D(handle);
}

glm::ivec2 Graphics::GetWindowSize()
{
    return GraphicsCore::GetWindowSize();
}