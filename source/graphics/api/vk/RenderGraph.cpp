#include "graphics/api/vk/MemoryManager.hpp"
#include <graphics/api/vk/RenderGraph.hpp>

using namespace Eve::Graphics;

TransientTextureHandle RenderGraph::RequestNewTransientTexture1D(TransientTextureInfo1D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();

    TextureInfo data
    {
        .Width = textureInfo.Width, 
        .Height = 1,
        .Depth = 1,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);

    return handle;
}

TransientTextureHandle RenderGraph::RequestNewTransientTexture2D(TransientTextureInfo2D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();
    TextureInfo data
    {
        .Width = textureInfo.Width, 
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);
    return handle;
}

TransientTextureHandle RenderGraph::RequestNewTransientTexture3D(TransientTextureInfo3D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();
    TextureInfo data
    {
        .Width = textureInfo.Width, 
        .Height = textureInfo.Height,
        .Depth = textureInfo.Depth,
        .ArrayLayers = textureInfo.ArrayLayers,
        .MipLevels = textureInfo.MipLevels,
        .Format = textureInfo.Format,
        .Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);
    return handle;
}

TransientBufferHandle RenderGraph::RequestNewTransientBuffer(TransientBufferInfo bufferInfo)
{
    TransientBufferHandle handle = MemoryManager::ReserveTransientBufferSlot();

    BufferInfo data
    {
        .Size = bufferInfo.Size
    };

    requestedBuffers.push_back(data);
    return handle;
}

void RenderGraph::AddPass(GraphicsPass* pass)
{
    passes.emplace_back(
        pass->GetTextures(),
        pass->GetBuffers()
    );
}

void RenderGraph::AddPass(TransferPass* pass)
{
    passes.emplace_back(
        pass->GetTextures(),
        pass->GetBuffers()
    );
}

void RenderGraph::AddPass(ComputePass* pass)
{
    passes.emplace_back(
        pass->GetTextures(),
        pass->GetBuffers()
    );
}

void RenderGraph::CompileGraph(uint32_t frameIndex)
{
    
    // Find the first and last pass for each resource
    // Calculate the resource usage so it can be used to create/reuse a resource
    for (uint32_t textureId = 0; textureId < requestedTextures.size(); textureId++)
    {
        int32_t firstPass = -1;
        int32_t lastPass = -1;
        TextureUsage usage = static_cast<TextureUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passes.size(); passIndex++)
        {
            Pass& pass = passes[passIndex];

            for (uint32_t i = 0; i < pass.Textures.size(); i++)
            {
                std::pair<TransientTextureHandle, Usage> resource = pass.Textures[i];
                if(resource.first.Id != textureId) { continue; }

                TextureUsage _usage = GetTextureUsage(resource.second);
                usage |= _usage;

                lastPass = passIndex;

                if(firstPass == -1)
                {
                    firstPass = passIndex;
                }
            }
        }

        if(firstPass == -1) { continue; } 
        passes[firstPass].TexturesToCreate.emplace_back(textureId);
        passes[lastPass].TexturesToDestroy.emplace_back(textureId);

        requestedTextures[textureId].Usage = usage;
    }
    for (uint32_t bufferId = 0; bufferId < requestedBuffers.size(); bufferId++)
    {
        int32_t firstPass = -1;
        int32_t lastPass = -1;
        BufferUsage usage = static_cast<BufferUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passes.size(); passIndex++)
        {
            Pass& pass = passes[passIndex];

            for (uint32_t i = 0; i < pass.Buffers.size(); i++)
            {
                std::pair<TransientBufferHandle, Usage> resource = pass.Buffers[i];
                if(resource.first.Id != bufferId) { continue; }

                BufferUsage _usage = GetBufferUsage(resource.second);
                usage |= _usage;

                lastPass = passIndex;

                if(firstPass == -1)
                {
                    firstPass = passIndex;
                }
            }
        }

        if(firstPass == -1) { continue; } 
        passes[firstPass].BuffersToCreate.emplace_back(bufferId);
        passes[lastPass].BuffersToDestroy.emplace_back(bufferId);

        requestedBuffers[bufferId].Usage = usage;
    }

    
    
    VmaVirtualBlockCreateInfo textureVirtualBlockCI
    {
        .size = texturePoolSize
    };

    VmaVirtualBlockCreateInfo bufferVirtualBlockCI
    {
        .size = bufferPoolSize
    };

    VmaVirtualBlock textureBlock;
    VmaVirtualBlock bufferBlock;
    vmaCreateVirtualBlock(&textureVirtualBlockCI, &textureBlock);
    vmaCreateVirtualBlock(&bufferVirtualBlockCI,  &bufferBlock);
    for (uint32_t i = 0; i < passes.size(); i++)
    {
        Pass& pass = passes[i];

        for(uint32_t i = 0; i < pass.TexturesToCreate.size(); i++)
        {
            
            VmaVirtualAllocationCreateInfo allocInfo
            {
                
            };
            //vmaVirtualAllocate(&virtualBlock)
        }

    }

    for(uint32_t i = 0; i < passes.size(); i++)
    {
        SyncPoint syncPoint;
        syncPoints.push_back(syncPoint);
    }

    for (uint32_t textureId = 0; textureId < requestedTextures.size(); textureId++)
    {
        TextureBarrierInfo oldBarrierInfo
        {
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            VK_ACCESS_2_NONE,
            VK_IMAGE_LAYOUT_UNDEFINED 
        };

        int32_t firstPass = -1;
        int32_t lastPass = -1;
        bool wasReadOnly = false;
        for(uint32_t passIndex = 0; passIndex < passes.size(); passIndex++)
        {
            Pass& pass = passes[passIndex];

            for (uint32_t i = 0; i < pass.Textures.size(); i++)
            {
                
                std::pair<TransientTextureHandle, Usage> resource = pass.Textures[i];
                if(resource.first.Id != textureId) { continue; }

                if(resource.second == Usage::NONE) { break; }

                // Set the last pass which uses this texture
                lastPass = passIndex;

                TextureBarrierInfo newBarrierInfo = CalculateTextureBarrierInfo(resource.second);

                bool isReadOnly = IsOnlyRead(resource.second);
                if(isReadOnly && wasReadOnly && oldBarrierInfo.Layout == newBarrierInfo.Layout)
                {
                    oldBarrierInfo.StageMask |= newBarrierInfo.StageMask;
                    oldBarrierInfo.AccessMask |= newBarrierInfo.AccessMask;
                    // This operation is the same as the last one and it is read-only
                    // So there is no need of a barrier
                    break;
                }
                
                VkImageMemoryBarrier2 barrier
                {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                    .srcStageMask = oldBarrierInfo.StageMask,
                    .srcAccessMask = oldBarrierInfo.AccessMask,

                    .dstStageMask = newBarrierInfo.StageMask,
                    .dstAccessMask = newBarrierInfo.AccessMask,

                    .oldLayout = oldBarrierInfo.Layout,
                    .newLayout = newBarrierInfo.Layout,
                };

                syncPoints[passIndex].imageBarriers.push_back(barrier);

                wasReadOnly = isReadOnly;
                oldBarrierInfo = newBarrierInfo;

                // Set the first pass which uses this texture
                if(firstPass == -1)
                {
                    firstPass = passIndex;
                }

                break;
            }

        }
    }

    for (uint32_t bufferId = 0; bufferId < requestedBuffers.size(); bufferId++)
    {
        BufferBarrierInfo oldBarrierInfo
        {
            VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
            VK_ACCESS_2_NONE
        };

        int32_t firstPass = -1;
        int32_t lastPass = -1;
        bool wasReadOnly = false;
        for(uint32_t passIndex = 0; passIndex < passes.size(); passIndex++)
        {
            Pass& pass = passes[passIndex];

            for (uint32_t i = 0; i < pass.Buffers.size(); i++)
            {
                
                std::pair<TransientBufferHandle, Usage> resource = pass.Buffers[i];
                if(resource.first.Id != bufferId) { continue; }

                if(resource.second == Usage::NONE) { break; }

                // Set the last pass which uses this buffer
                lastPass = passIndex;

                BufferBarrierInfo newBarrierInfo = CalculateBufferBarrierInfo(resource.second);
                

                bool isReadOnly = IsOnlyRead(resource.second);
                if(isReadOnly && wasReadOnly)
                { 
                    // This operation is the same as the last one and it is read-only
                    // So there is no need of a barrier

                    oldBarrierInfo.StageMask |= newBarrierInfo.StageMask;
                    oldBarrierInfo.AccessMask |= newBarrierInfo.AccessMask;

                    break;
                }

                VkBufferMemoryBarrier2 barrier
                {
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                    .srcStageMask = oldBarrierInfo.StageMask,
                    .srcAccessMask = oldBarrierInfo.AccessMask,

                    .dstStageMask = newBarrierInfo.StageMask,
                    .dstAccessMask = newBarrierInfo.AccessMask
                };

                syncPoints[passIndex].bufferBarriers.push_back(barrier);

                wasReadOnly = isReadOnly;
                oldBarrierInfo = newBarrierInfo;

                // Set the first pass which uses this buffer
                if(firstPass == -1)
                {
                    firstPass = passIndex;
                }

                break;
            }

        }
    }
}

RenderGraph::TextureBarrierInfo RenderGraph::CalculateTextureBarrierInfo(Usage usage)
{
    switch(usage)
    {
        case(Usage::VERTEX_READ_TEXTURE_SAMPLED) :
            return 
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::FRAGMENT_READ_TEXTURE_SAMPLED) :
            return
            {
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_SAMPLED_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::VERTEX_READ_TEXTURE_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::FRAGMENT_READ_TEXTURE_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::VERTEX_FRAGMENT_READ_TEXTURE_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::COMPUTE_READ_TEXTURE_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

        case(Usage::COMPUTE_WRITE_TEXTURE_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
                VK_IMAGE_LAYOUT_GENERAL
            };
        case(Usage::COLOR_ATTACHMENT) :
            return
            {
                VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            };

        case(Usage::DEPTH_STENCIL_READ_ONLY) :
            return
            {
                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
            };

        case(Usage::DEPTH_STENCIL_WRITE) :
            return
            {
                VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            };

        case(Usage::COPY_SOURCE) :
            return
            {
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_TRANSFER_READ_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
            };

        case(Usage::COPY_DESTINATION) :
            return
            {
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            };

        default:
            return
            {
                VK_PIPELINE_STAGE_2_NONE,
                VK_ACCESS_2_NONE,
                VK_IMAGE_LAYOUT_UNDEFINED
            };
    }
}

RenderGraph::BufferBarrierInfo RenderGraph::CalculateBufferBarrierInfo(Usage usage)
{
    switch(usage)
    {
        case(Usage::BUFFER_INDEX_READ_ONLY) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                VK_ACCESS_2_INDEX_READ_BIT
            };

        case(Usage::VERTEX_READ_BUFFER_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT
            };
        
        case(Usage::VERTEX_READ_BUFFER_UNIFORM) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
                VK_ACCESS_2_UNIFORM_READ_BIT
            };

        case(Usage::FRAGMENT_READ_BUFFER_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT
            };
        
        case(Usage::FRAGMENT_READ_BUFFER_UNIFORM) :
            return
            {
                VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_UNIFORM_READ_BIT
            };

        case(Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT
            };
        
        case(Usage::VERTEX_FRAGMENT_READ_BUFFER_UNIFORM) :
            return
            {
                VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                VK_ACCESS_2_UNIFORM_READ_BIT
            };
        
        case(Usage::COMPUTE_READ_BUFFER_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_READ_BIT
            };

        case(Usage::COMPUTE_READ_BUFFER_UNIFORM) :
            return
            {
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                VK_ACCESS_2_UNIFORM_READ_BIT
            };
        
        case(Usage::COMPUTE_WRITE_BUFFER_STORAGE) :
            return
            {
                VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT
            };

        case(Usage::COPY_SOURCE) :
            return
            {
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_TRANSFER_READ_BIT,
            };

        case(Usage::COPY_DESTINATION) :
            return
            {
                VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                VK_ACCESS_2_TRANSFER_WRITE_BIT,
            };

        default:
            return
            {
                VK_PIPELINE_STAGE_2_NONE,
                VK_ACCESS_2_NONE,
            };
        
    }
}

TextureUsage RenderGraph::GetTextureUsage(Usage usage)
{
    switch (usage)
    {
        case (Usage::VERTEX_READ_TEXTURE_SAMPLED) :
            return TextureUsage::USAGE_SAMPLED;

        case (Usage::FRAGMENT_READ_TEXTURE_SAMPLED) :
            return TextureUsage::USAGE_SAMPLED;
        
        case (Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED) :
            return TextureUsage::USAGE_SAMPLED;
        
        case (Usage::VERTEX_READ_TEXTURE_STORAGE) :
            return TextureUsage::USAGE_STORAGE;

        case (Usage::FRAGMENT_READ_TEXTURE_STORAGE) :
            return TextureUsage::USAGE_STORAGE;

        case (Usage::VERTEX_FRAGMENT_READ_TEXTURE_STORAGE) :
            return TextureUsage::USAGE_STORAGE;

        case (Usage::COMPUTE_READ_TEXTURE_STORAGE) :
            return TextureUsage::USAGE_STORAGE;

        case (Usage::COMPUTE_WRITE_TEXTURE_STORAGE) :
            return TextureUsage::USAGE_STORAGE;

        case (Usage::COLOR_ATTACHMENT) :
            return TextureUsage::USAGE_COLOR_ATTACHMENT; 

        case (Usage::DEPTH_STENCIL_READ_ONLY) :
            return TextureUsage::USAGE_DEPTH_STENCIL_ATTACHMENT; 

        case (Usage::DEPTH_STENCIL_WRITE) :
            return TextureUsage::USAGE_DEPTH_STENCIL_ATTACHMENT; 

        case (Usage::COPY_SOURCE) :
            return TextureUsage::USAGE_TRANSFER_SRC;

        case (Usage::COPY_DESTINATION) :
            return TextureUsage::USAGE_TRANSFER_DST;

        default : return static_cast<TextureUsage>(0);
    }
}

BufferUsage RenderGraph::GetBufferUsage(Usage usage)
{
    switch (usage)
    {
        case (Usage::BUFFER_INDEX_READ_ONLY) : 
            return BufferUsage::BUFFER_USAGE_INDEX;
        case(Usage::VERTEX_READ_BUFFER_STORAGE) :
            return BufferUsage::BUFFER_USAGE_STORAGE;
        
        case(Usage::VERTEX_READ_BUFFER_UNIFORM) :
            return BufferUsage::BUFFER_USAGE_UNIFORM;
        
        case(Usage::FRAGMENT_READ_BUFFER_STORAGE) :
            return BufferUsage::BUFFER_USAGE_STORAGE;

        case(Usage::FRAGMENT_READ_BUFFER_UNIFORM) :
            return BufferUsage::BUFFER_USAGE_UNIFORM;
        
        case(Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE) :
            return BufferUsage::BUFFER_USAGE_STORAGE;
        
        case(Usage::VERTEX_FRAGMENT_READ_BUFFER_UNIFORM) :
            return BufferUsage::BUFFER_USAGE_UNIFORM;

        case(Usage::COMPUTE_READ_BUFFER_STORAGE) :
            return BufferUsage::BUFFER_USAGE_STORAGE;

        case(Usage::COMPUTE_READ_BUFFER_UNIFORM) :
            return BufferUsage::BUFFER_USAGE_UNIFORM;

        case(Usage::COMPUTE_WRITE_BUFFER_STORAGE) :
            return BufferUsage::BUFFER_USAGE_STORAGE;

        case(Usage::COPY_SOURCE) :
            return BufferUsage::BUFFER_USAGE_TRANSFER_SRC;

        case(Usage::COPY_DESTINATION) :
            return BufferUsage::BUFFER_USAGE_TRANSFER_SRC;

        default: return static_cast<BufferUsage>(0);
    }
}

bool RenderGraph::IsOnlyRead(Usage usage)
{
    switch (usage) 
    {
        case(Usage::COMPUTE_WRITE_TEXTURE_STORAGE) :
            return false;

        case(Usage::COMPUTE_WRITE_BUFFER_STORAGE) :
            return false;

        case(Usage::COLOR_ATTACHMENT) :
            return false;
        
        case(Usage::DEPTH_STENCIL_WRITE) :
            return false;

        case(Usage::COPY_DESTINATION) :
            return false;
        
        default:
            return true;
    }
}

void RenderGraph::Initialize()
{
    for(uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
    {
        VmaPool texturePool = MemoryManager::AllocateMemoryPool(Eve::Settings::transientDefaultTexturesPoolSize);
        VmaPool bufferPool = MemoryManager::AllocateMemoryPool(Eve::Settings::transientDefaultBuffersPoolSize);

        texturePoolsInUse.push_back(texturePool);
        bufferPoolsInUse.push_back(bufferPool);
    }
}