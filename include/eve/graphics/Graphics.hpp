#pragma once

#include "RenderViewHandle.hpp"
#include <eve/graphics/Buffer.hpp>
#include <eve/graphics/Sampler.hpp>
#include <eve/graphics/Texture.hpp>
#include <eve/graphics/Mesh.hpp>
#include <eve/graphics/ShaderHandle.hpp>
#include <eve/graphics/Pass.hpp>

namespace Eve::Graphics 
{
    class Graphics
    {
        public:
            // --- Texture Creation ---
            static TextureHandle CreateTexture1D(TextureInfo1D textureInfo);
            static TextureHandle CreateTexture2D(TextureInfo2D textureInfo);
            static TextureHandle CreateTexture3D(TextureInfo3D textureInfo);
            static TextureHandle CreateTextureCube(TextureInfo2D textureInfo);

            // --- Sampler Creation ---
            static SamplerHandle CreateSampler(SamplerInfo samplerInfo);

            // --- Buffer Creation ---
            static BufferHandle CreateGPUBuffer(uint64_t size);

            // --- Resource Destruction ---
            static void DestroyBuffer(BufferHandle handle);
            static void DestroyTexture(TextureHandle handle);
            static void DestroySampler(SamplerHandle handle);

            // --- Meshes ---
            static MeshHandle CreateMesh();
            static void DestroyMesh(MeshHandle handle);
            static CPUMesh& GetCPUMesh(MeshHandle handle);
            static GraphicsMesh& GetGraphicsMesh(MeshHandle handle);

            // --- Shaders & Materials ---
            static ShaderHandle CreateGraphicsShader(ShaderInfo shaderInfo);
            static MaterialHandle CreateMaterial(ShaderHandle handle);

            static RenderViewHandle CreateRenderView();
            static void DestroyRenderView(RenderViewHandle handle);

            // --- Render Graph ---
            static TransientTextureHandle RequestTransientTexture1D(TransientTextureInfo1D textureInfo);
            static TransientTextureHandle RequestTransientTexture2D(TransientTextureInfo2D textureInfo);
            static TransientTextureHandle RequestTransientTexture3D(TransientTextureInfo3D textureInfo);
            static TransientTextureHandle RequestTransientTextureCube(TransientTextureInfo2D textureInfo);
            static TransientBufferHandle RequestTransientBuffer(uint64_t size);            
            static void AddPass(GraphicsPass& pass);
            static void AddPass(TransferPass& pass);
            static void AddPass(ComputePass& pass);
            static void AddPass(GraphicsPass& pass, uint32_t index);
            static void AddPass(TransferPass& pass, uint32_t index);
            static void AddPass(ComputePass& pass, uint32_t index);

            static void SetPresentTexture2D(TransientTextureHandle handle);

            static glm::ivec2 GetWindowSize();



    };
}