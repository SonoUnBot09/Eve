#include <graphics/api/vk/RenderGraphA.hpp>

using namespace Eve::Graphics;

namespace 
{
    TextureUsage GetTextureUsage(Usage usage)
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
    BufferUsage GetBufferUsage(Usage usage)
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

    bool IsReadOnly(Usage usage)
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

    RenderGraph::TextureBarrierInfo CalculateTextureBarrierInfo(Usage usage)
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
                    VK_IMAGE_LAYOUT_GENERAL
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
    RenderGraph::BufferBarrierInfo CalculateBufferBarrierInfo(Usage usage)
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

    bool IsTextureBarrierNeeded(RenderGraph::TextureBarrierInfo src, RenderGraph::TextureBarrierInfo dst, Usage oldUsage, Usage newUsage)
    {
        if(IsReadOnly(oldUsage) && IsReadOnly(newUsage))
        {
            if(src.Layout == dst.Layout)
            {
                return false;
            }
            else 
            {
                return true;
            }
        }
        else 
        {
            return true;
        }
    }

    bool IsBufferBarrierNeeded(RenderGraph::BufferBarrierInfo src, RenderGraph::BufferBarrierInfo dst, Usage oldUsage, Usage newUsage)
    {
        if(IsReadOnly(oldUsage) && IsReadOnly(newUsage))
        {
            return false;
        }
        else 
        {
            return true;
        }
    }

    uint32_t FindBestMemoryTypeIndex(VkMemoryRequirements2& memoryRequirements)
    {
        bool isDedicatedGPU = ContextBuilder::context.PhysicalDeviceInfo.isDedicated;
        int32_t score = INT32_MIN;
        int32_t memoryTypeIndex = 0;
        for(uint32_t i = 0; i < ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypeCount; i++)
        {
            bool isCompatibleBit = (memoryRequirements.memoryRequirements.memoryTypeBits & (1 << i)) != 0;

            if(!isCompatibleBit) { continue; }

            int32_t currentScore = 0;

            // Local Device?
            if(ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                currentScore += 500;
            }

            // Host Visible?
            if(isDedicatedGPU && (ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            {
                currentScore -= 200;
            }

            // Host Coherent?
            if(isDedicatedGPU && (ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                currentScore -= 150;
            }

            // Lazily Allocated?
            if((ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
            {
                currentScore -= 1000;
            }

            // Some kink of memory which is not DEVICE_LOCAL, HOST_VISIBLE and HOST_COHERENT
            if((ContextBuilder::context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags &
                ~(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) > 0 )
            {
                currentScore -= 5000;
            }

            if(currentScore > score)
            {
                memoryTypeIndex = i;
                score = currentScore;
            }
        }

        return memoryTypeIndex;
    };
};

void RenderGraph::CompileGraph(uint32_t frameIndex)
{
    uint32_t texturesCount = requestedTextures.size();
    uint32_t buffersCount = requestedBuffers.size();
    uint32_t passesCount = passes.size();

    // Calculate first and last passes
    // Calculate resources usage
    // Calculate resource memory info
    uint32_t texturesBarriersOffset = 0;
    barriersOffsetPerTexture.push_back(texturesBarriersOffset);
    for(uint32_t textureId = 0; textureId < texturesCount; textureId++)
    {
        uint32_t barriersCount = 0;
        int32_t firstPassIndex = -1;
        int32_t lastPassIndex = -1;

        TextureBarrierInfo srcBarrierInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Layout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        Usage oldUsage = static_cast<Usage>(0);
        TextureUsage usage = static_cast<TextureUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientTextureHandle, Usage>>& textures = passes[passIndex].Textures;
            for(uint32_t i = 0; i < textures.size(); i++)
            {
                // For each textures in that pass grab the texture usage
                // and calculate the first and last passes
                if(textures[i].first.Id != textureId) { continue; }

                if(firstPassIndex == -1)
                {
                    firstPassIndex = passIndex;
                }

                lastPassIndex = passIndex;

                usage |= GetTextureUsage(textures[i].second);

                TextureBarrierInfo dstBarrierInfo = CalculateTextureBarrierInfo(textures[i].second);

                bool barrier = IsTextureBarrierNeeded(srcBarrierInfo, dstBarrierInfo, oldUsage, textures[i].second);

                if(barrier)
                {
                    texturesBarriersInfo.push_back(std::pair{dstBarrierInfo, passIndex});

                    srcBarrierInfo = dstBarrierInfo;

                    barriersCount++;
                }
                else 
                {
                    srcBarrierInfo.StageMask |= dstBarrierInfo.StageMask;
                    srcBarrierInfo.AccessMask |= dstBarrierInfo.AccessMask;
                }

                oldUsage = textures[i].second;

                break;
            }
        }

        texturesBarriersOffset += barriersCount;
        barriersOffsetPerTexture.push_back(texturesBarriersOffset);

        if(firstPassIndex == -1) { continue; }

        transientTextures[frameIndex][textureId].TextureInfo = requestedTextures[textureId];
        transientTextures[frameIndex][textureId].TextureInfo.Data.Usage = usage;

        uint32_t bucketIndex = SetTextureMemoryInfo(frameIndex, textureId, passesCount);

        texturesBucketPasses[bucketIndex][firstPassIndex].TexturesToCreate.emplace_back(textureId);
        texturesBucketPasses[bucketIndex][lastPassIndex].TexturesToDestroy.emplace_back(textureId);
    }
    uint32_t buffersBarriersOffset = 0;
    barriersOffsetPerBuffer.push_back(buffersBarriersOffset);
    for(uint32_t bufferId = 0; bufferId < buffersCount; bufferId++)
    {
        uint32_t barriersCount = 0;
        int32_t firstPassIndex = -1;
        int32_t lastPassIndex = -1;

        BufferBarrierInfo srcBarrierInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE
        };

        Usage oldUsage = static_cast<Usage>(0);
        BufferUsage usage = static_cast<BufferUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientBufferHandle, Usage>>& buffers = passes[passIndex].Buffers;
            for(uint32_t i = 0; i < buffers.size(); i++)
            {
                // For each textures in that pass grab the texture usage
                // and calculate the first and last passes
                if(buffers[i].first.Id != bufferId) { continue; }

                if(firstPassIndex == -1)
                {
                    firstPassIndex = passIndex;
                }

                lastPassIndex = passIndex;

                usage |= GetBufferUsage(buffers[i].second);

                BufferBarrierInfo dstBarrierInfo = CalculateBufferBarrierInfo(buffers[i].second);

                bool firstUse = firstPassIndex == passIndex;
                bool barrier = firstUse || IsBufferBarrierNeeded(srcBarrierInfo, dstBarrierInfo, oldUsage, buffers[i].second);

                if(barrier)
                {
                    buffersBarriersInfo.push_back(std::pair{dstBarrierInfo, passIndex});

                    srcBarrierInfo = dstBarrierInfo;

                    barriersCount++;
                }
                else 
                {
                    srcBarrierInfo.StageMask |= dstBarrierInfo.StageMask;
                    srcBarrierInfo.AccessMask |= dstBarrierInfo.AccessMask;
                }

                oldUsage = buffers[i].second;

                break;
            }
        }

        buffersBarriersOffset += barriersCount;
        barriersOffsetPerBuffer.push_back(buffersBarriersOffset);

        if(firstPassIndex == -1) { continue; }

        transientBuffers[frameIndex][bufferId].BufferInfo = requestedBuffers[bufferId];
        transientBuffers[frameIndex][bufferId].BufferInfo.Data.Usage = usage;

        uint32_t bucketIndex = SetBufferMemoryInfo(frameIndex, bufferId, passesCount);

        buffersBucketPasses[bucketIndex][firstPassIndex].BuffersToCreate.emplace_back(bufferId);
        buffersBucketPasses[bucketIndex][lastPassIndex].BuffersToDestroy.emplace_back(bufferId);

    }

    VmaVirtualBlockCreateInfo virtualBlockCI
    {
        .size = UINT64_MAX
    };

    VmaVirtualBlock block;
    vmaCreateVirtualBlock(&virtualBlockCI, &block);

    texturesVirtualAllocs.resize(texturesCount);
    buffersVirtualAllocs.resize(buffersCount);

    // Textures Memory Aliasing
    for(uint32_t bucketIndex = 0; bucketIndex < texturesBucketPasses.size(); bucketIndex++)
    {
        uint64_t peakSize = 0;
        uint64_t peakAlignment = 0;
        std::vector<TexturesBucketPass>& _passes = texturesBucketPasses[bucketIndex];

        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            TexturesBucketPass& pass = _passes[passIndex];

            // Texture Creation
            std::vector<TransientTextureHandle>& texturesToCreate = pass.TexturesToCreate;

            for(uint32_t i = 0; i < texturesToCreate.size(); i++)
            {
                TransientTextureHandle handle = texturesToCreate[i];
                TextureResource& texture = transientTextures[frameIndex][handle.Id];

                VmaVirtualAllocationCreateInfo allocCI
                {
                    .size = texture.MemoryInfo.Size,
                    .alignment = texture.MemoryInfo.Alignment
                };

                VmaVirtualAllocation allocation;
                VkDeviceSize offset;
                vmaVirtualAllocate(block, &allocCI, &allocation, &offset);

                peakSize = std::max(peakSize, static_cast<uint64_t>(offset) + texture.MemoryInfo.Size);
                peakAlignment = std::max(peakAlignment, texture.MemoryInfo.Alignment);

                TextureBarrierInfo firstBarrier = GetFirstTextureBarrierInfo(handle.Id, offset, texture.MemoryInfo.Size);
                uint32_t firstBarrierIndex = barriersOffsetPerTexture[handle.Id];
                texturesBarriersInfo[firstBarrierIndex].first = firstBarrier;

                texturesVirtualAllocs[handle.Id] = allocation;

                texture.TextureInfo.MemoryInfo.Offset = static_cast<uint64_t>(offset);
            }

            // Texture Destruction
            std::vector<TransientTextureHandle>& texturesToDestroy = pass.TexturesToDestroy;

            for(uint32_t i = 0; i < texturesToDestroy.size(); i++)
            {
                TransientTextureHandle handle = texturesToDestroy[i];

                VmaVirtualAllocation allocation = texturesVirtualAllocs[handle.Id];

                vmaVirtualFree(block, allocation);
            }

        }

        vmaClearVirtualBlock(block);
        virtualMemorySlots.clear();

        bool success = ResizeTexturePoolIfNeeded(bucketIndex, peakSize, peakAlignment);

        if(!success)
        {
            return;
        }
    }

    // Buffers Memory Aliasing
    for (uint32_t bucketIndex = 0; bucketIndex < buffersBucketPasses.size(); bucketIndex++)
    {
        uint64_t peakSize = 0;
        uint64_t peakAlignment = 0;
        std::vector<BuffersBucketPass>& _passes = buffersBucketPasses[bucketIndex];

        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            BuffersBucketPass& pass = _passes[passIndex];

            // Buffer Creation
            std::vector<TransientBufferHandle>& buffersToCreate = pass.BuffersToCreate;

            for(uint32_t i = 0; i < buffersToCreate.size(); i++)
            {
                TransientBufferHandle handle = buffersToCreate[i];
                BufferResource& buffer = transientBuffers[frameIndex][handle.Id];

                VmaVirtualAllocationCreateInfo allocCI
                {
                    .size = buffer.MemoryInfo.Size,
                    .alignment = buffer.MemoryInfo.Alignment
                };

                VmaVirtualAllocation allocation;
                VkDeviceSize offset;
                vmaVirtualAllocate(block, &allocCI, &allocation, &offset);

                peakSize = std::max(peakSize, static_cast<uint64_t>(offset) + buffer.MemoryInfo.Size);
                peakAlignment = std::max(peakAlignment, buffer.MemoryInfo.Alignment);

                BufferBarrierInfo firstBarrier = GetFirstBufferBarrierInfo(handle.Id, offset, buffer.MemoryInfo.Size);
                uint32_t firstBarrierIndex = barriersOffsetPerBuffer[handle.Id];
                buffersBarriersInfo[firstBarrierIndex].first = firstBarrier;

                buffersVirtualAllocs[handle.Id] = allocation;

                buffer.BufferInfo.MemoryInfo.Offset = static_cast<uint64_t>(offset);
            }

            // Buffer Destruction
            std::vector<TransientBufferHandle>& buffersToDestroy = pass.BuffersToDestroy;

            for(uint32_t i = 0; i < buffersToDestroy.size(); i++)
            {
                TransientBufferHandle handle = buffersToDestroy[i];

                VmaVirtualAllocation allocation = buffersVirtualAllocs[handle.Id];

                vmaVirtualFree(block, allocation);
            }
        }

        vmaClearVirtualBlock(block);
        virtualMemorySlots.clear();

        bool success = ResizeBufferPoolIfNeeded(bucketIndex, peakSize, peakAlignment);

        if(!success)
        {
            return;
        }

    }

    vmaDestroyVirtualBlock(block);
}

uint32_t RenderGraph::SetTextureMemoryInfo(const uint32_t frameIndex, const uint32_t textureId, const uint32_t passesCount)
{
    TextureResource& resource = transientTextures[frameIndex][textureId];

    auto it = std::lower_bound(texturesPool.begin(), texturesPool.end(), resource, 
    [](const std::pair<TextureResource, bool>& a, const TextureResource& b)
    {
        return std::memcmp(&a.first.TextureInfo.Data, &b.TextureInfo.Data, sizeof(TextureInfo::Data)) < 0;
    });

    if(it != texturesPool.end() && std::memcmp(&it->first.TextureInfo.Data, &resource.TextureInfo.Data, sizeof(TextureInfo::Data)) == 0)
    {
        // Texture with same setting found
        resource.MemoryInfo.BucketIndex = it->first.MemoryInfo.BucketIndex;
        resource.MemoryInfo.Alignment = static_cast<uint64_t>(it->first.MemoryInfo.Alignment);
        resource.MemoryInfo.Size = static_cast<uint64_t>(it->first.MemoryInfo.Size);

        resource.TextureInfo.MemoryInfo.Size = static_cast<uint64_t>(it->first.MemoryInfo.Size);

        return it->first.MemoryInfo.BucketIndex;
    }

    VkImageType imageType = VK_IMAGE_TYPE_2D;
    #pragma region Determine Image Type
    if(resource.TextureInfo.Data.Depth > 1)
    {
        imageType = VK_IMAGE_TYPE_3D;
    }
    else if(resource.TextureInfo.Data.Height == 1)
    {
        imageType = VK_IMAGE_TYPE_1D;
    }
    #pragma endregion

    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = imageType,
        .format = GetVkImageFormat(resource.TextureInfo.Data.Format),
        .extent {
            .width = resource.TextureInfo.Data.Width,
            .height = resource.TextureInfo.Data.Height,
            .depth = resource.TextureInfo.Data.Depth
        },
        .mipLevels = resource.TextureInfo.Data.MipLevels,
        .arrayLayers = resource.TextureInfo.Data.ArrayLayers,
        .samples = GetVkImageSamplesCount(resource.TextureInfo.Data.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(resource.TextureInfo.Data.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkDeviceImageMemoryRequirements reqs
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS,
        .pCreateInfo = &imageCI
    };

    VkMemoryRequirements2 memoryRequirements { .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetDeviceImageMemoryRequirements(ContextBuilder::context.Device, &reqs, &memoryRequirements);

    uint32_t memoryTypeIndex = FindBestMemoryTypeIndex(memoryRequirements);

    uint32_t bucketIndex = GetTexturesBucketIndex(memoryTypeIndex, passesCount);

    resource.MemoryInfo.BucketIndex = bucketIndex;
    resource.MemoryInfo.Alignment = static_cast<uint64_t>(memoryRequirements.memoryRequirements.alignment);
    resource.MemoryInfo.Size = static_cast<uint64_t>(memoryRequirements.memoryRequirements.size);

    resource.TextureInfo.MemoryInfo.Size = static_cast<uint64_t>(memoryRequirements.memoryRequirements.size);

    return bucketIndex;
}

uint32_t RenderGraph::SetBufferMemoryInfo(const uint32_t frameIndex, const uint32_t bufferId, const uint32_t passesCount)
{
    BufferResource& resource = transientBuffers[frameIndex][bufferId];

    auto it = std::lower_bound(buffersPool.begin(), buffersPool.end(), resource, 
    [](const std::pair<BufferResource, bool>& a, const BufferResource& b)
    {
        return std::memcmp(&a.first.BufferInfo.Data, &b.BufferInfo.Data, sizeof(BufferInfo::Data)) < 0;
    });

    if(it != buffersPool.end() && std::memcmp(&it->first.BufferInfo.Data, &resource.BufferInfo.Data, sizeof(BufferInfo::Data)) == 0)
    {
        // Texture with same setting found
        resource.MemoryInfo.BucketIndex = it->first.MemoryInfo.BucketIndex;
        resource.MemoryInfo.Alignment = static_cast<uint64_t>(it->first.MemoryInfo.Alignment);
        resource.MemoryInfo.Size = static_cast<uint64_t>(it->first.MemoryInfo.Size);

        resource.BufferInfo.MemoryInfo.Size = static_cast<uint64_t>(it->first.MemoryInfo.Size);

        return it->first.MemoryInfo.BucketIndex;
    }

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = resource.BufferInfo.Data.Size,
        .usage = GetVkBufferUsage(resource.BufferInfo.Data.Usage) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VkDeviceBufferMemoryRequirements reqs
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
        .pCreateInfo = &bufferCI
    };

    VkMemoryRequirements2 memoryRequirements { .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2 };
    vkGetDeviceBufferMemoryRequirements(ContextBuilder::context.Device, &reqs, &memoryRequirements);

    uint32_t memoryTypeIndex = FindBestMemoryTypeIndex(memoryRequirements);

    uint32_t bucketIndex = GetBuffersBucketIndex(memoryTypeIndex, passesCount);

    resource.MemoryInfo.BucketIndex = bucketIndex;
    resource.MemoryInfo.Alignment = static_cast<uint64_t>(memoryRequirements.memoryRequirements.alignment);
    resource.MemoryInfo.Size = static_cast<uint64_t>(memoryRequirements.memoryRequirements.size);

    resource.BufferInfo.MemoryInfo.Size = static_cast<uint64_t>(memoryRequirements.memoryRequirements.size);

    return bucketIndex;
}

uint32_t RenderGraph::GetTexturesBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount)
{
    uint32_t bucketsCount = texturesMemoryTypeIndicies.size();
    for (uint32_t i = 0; i < bucketsCount; i++)
    {
        if(texturesMemoryTypeIndicies[i] == memoryTypeIndex)
        {
            return i;
        }
    }

    texturesMemoryTypeIndicies.push_back(memoryTypeIndex);
    uint32_t bucketIndex = bucketsCount;

    texturesBucketPasses.push_back(std::vector<TexturesBucketPass>(passesCount));

    MemoryBucket memoryBucket
    {
        .used = false
    };
    texturesMemoryBucket.push_back(memoryBucket);

    return bucketIndex;
}

uint32_t RenderGraph::GetBuffersBucketIndex(const uint32_t memoryTypeIndex, const uint32_t passesCount)
{
    uint32_t bucketsCount = buffersMemoryTypeIndicies.size();
    for (uint32_t i = 0; i < bucketsCount; i++)
    {
        if(buffersMemoryTypeIndicies[i] == memoryTypeIndex)
        {
            return i;
        }
    }

    buffersMemoryTypeIndicies.push_back(memoryTypeIndex);
    uint32_t bucketIndex = bucketsCount;

    buffersBucketPasses.push_back(std::vector<BuffersBucketPass>(passesCount));

    MemoryBucket memoryBucket
    {
        .used = false
    };
    buffersMemoryBucket.push_back(memoryBucket);

    return bucketIndex;
}

uint32_t RenderGraph::GetTexturesMemoryTypeIndex(const uint32_t bucketIndex)
{
    return texturesMemoryTypeIndicies[bucketIndex];
}

uint32_t RenderGraph::GetBuffersMemoryTypeIndex(const uint32_t bucketIndex)
{
    return buffersMemoryTypeIndicies[bucketIndex];
}

RenderGraph::TextureBarrierInfo RenderGraph::GetFirstTextureBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize)
{
    TextureBarrierInfo barrier
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Layout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    uint64_t start = newAllocOffset;
    uint64_t end = newAllocOffset + newAllocSize;

    MemorySlot newMemorySlot{start, end, newAllocId};

    auto it = std::lower_bound(virtualMemorySlots.begin(), virtualMemorySlots.end(), newMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
        return a.End <= b.Start;
    });

    uint32_t firstSlotIndex = it - virtualMemorySlots.begin();

    uint32_t slotsCount = 0;
    for(; it != virtualMemorySlots.end(); it++)
    {
        if(it->Start >= newMemorySlot.End) { break; }

        uint32_t lastBarrierIndex = barriersOffsetPerTexture[it->ResourceId + 1] - 1;

        TextureBarrierInfo accumulatorBarrier = texturesBarriersInfo[lastBarrierIndex].first;

        barrier.StageMask |= accumulatorBarrier.StageMask;
        barrier.AccessMask |= accumulatorBarrier.AccessMask;

        slotsCount++;
    }

    if(slotsCount > 0)
    {
        uint32_t lastSlotIndex = (it - virtualMemorySlots.begin()) - 1;

        MemorySlot firstSlot = virtualMemorySlots[firstSlotIndex];
        MemorySlot lastSlot = virtualMemorySlots[lastSlotIndex];

        int64_t strideStart = newMemorySlot.Start - firstSlot.Start;
        int64_t strideEnd = lastSlot.End - newMemorySlot.End;

        MemorySlot fragments[3];
        uint32_t fragmentsCount = 0;

        if(strideStart > 0)
        {
            // Free memory at the beginnig of the slot
            MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceId};

            fragments[fragmentsCount++] = startSlot;
        }

        fragments[fragmentsCount++] = newMemorySlot;

        if(strideEnd > 0)
        {
            // Free memory at the end of the slot
            MemorySlot endSlot{newMemorySlot.End, newMemorySlot.End + strideEnd, lastSlot.ResourceId};

            fragments[fragmentsCount++] = endSlot;
        }

        virtualMemorySlots.erase(
            virtualMemorySlots.begin() + firstSlotIndex, 
            virtualMemorySlots.begin() + lastSlotIndex + 1
        );

        virtualMemorySlots.insert(virtualMemorySlots.begin() + firstSlotIndex, fragments, fragments + fragmentsCount);
    }
    else 
    {
        virtualMemorySlots.insert(virtualMemorySlots.begin() + firstSlotIndex, newMemorySlot);
    }

    return barrier;
}

RenderGraph::BufferBarrierInfo RenderGraph::GetFirstBufferBarrierInfo(const uint32_t newAllocId, const uint64_t newAllocOffset, const uint64_t newAllocSize)
{
    BufferBarrierInfo barrier
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
    };

    uint64_t start = newAllocOffset;
    uint64_t end = newAllocOffset + newAllocSize;

    MemorySlot newMemorySlot{start, end, newAllocId};

    auto it = std::lower_bound(virtualMemorySlots.begin(), virtualMemorySlots.end(), newMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
        return a.End <= b.Start;
    });

    uint32_t firstSlotIndex = it - virtualMemorySlots.begin();

    uint32_t slotsCount = 0;
    for(; it != virtualMemorySlots.end(); it++)
    {
        if(it->Start >= newMemorySlot.End) { break; }

        uint32_t lastBarrierIndex = barriersOffsetPerBuffer[it->ResourceId + 1] - 1;

        BufferBarrierInfo accumulatorBarrier = buffersBarriersInfo[lastBarrierIndex].first;

        barrier.StageMask |= accumulatorBarrier.StageMask;
        barrier.AccessMask |= accumulatorBarrier.AccessMask;

        slotsCount++;
    }

    if(slotsCount > 0)
    {
        uint32_t lastSlotIndex = (it - virtualMemorySlots.begin()) - 1;

        MemorySlot firstSlot = virtualMemorySlots[firstSlotIndex];
        MemorySlot lastSlot = virtualMemorySlots[lastSlotIndex];

        int64_t strideStart = newMemorySlot.Start - firstSlot.Start;
        int64_t strideEnd = lastSlot.End - newMemorySlot.End;

        MemorySlot fragments[3];
        uint32_t fragmentsCount = 0;

        if(strideStart > 0)
        {
            // Free memory at the beginnig of the slot
            MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceId};

            fragments[fragmentsCount++] = startSlot;
        }

        fragments[fragmentsCount++] = newMemorySlot;

        if(strideEnd > 0)
        {
            // Free memory at the end of the slot
            MemorySlot endSlot{newMemorySlot.End, newMemorySlot.End + strideEnd, lastSlot.ResourceId};

            fragments[fragmentsCount++] = endSlot;
        }

        virtualMemorySlots.erase(
            virtualMemorySlots.begin() + firstSlotIndex, 
            virtualMemorySlots.begin() + lastSlotIndex + 1
        );

        virtualMemorySlots.insert(virtualMemorySlots.begin() + firstSlotIndex, fragments, fragments + fragmentsCount);
    }
    else 
    {
        virtualMemorySlots.insert(virtualMemorySlots.begin() + firstSlotIndex, newMemorySlot);
    }

    return barrier;
}

bool RenderGraph::ResizeTexturePoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
{
    if(peakSize == 0) { return true; }

    MemoryBucket& memoryBucket = texturesMemoryBucket[bucketIndex];

    if(memoryBucket.used && memoryBucket.AllocationInfo.size >= peakSize)
    {
        return true;
    }

    for(int32_t i = static_cast<int32_t>(texturesPool.size()) - 1; i >= 0; i--)
    {
        std::pair<TextureResource, bool>& texture = texturesPool[i];

        if(texture.first.MemoryInfo.BucketIndex != bucketIndex) { continue; }

        vkDestroyImageView(ContextBuilder::context.Device, texture.first.Texture.ImageView, nullptr);
        vkDestroyImage(ContextBuilder::context.Device, texture.first.Texture.Image, nullptr);

        texturesPool.erase(texturesPool.begin() + i);
    }

    if(memoryBucket.used)
    {
        vmaFreeMemory(ContextBuilder::context.Allocator, memoryBucket.Allocation);
        memoryBucket.used = false;
    }

    VkMemoryRequirements memReqs
    {
        .size = peakSize + Eve::Settings::transientTexturesStepPoolSize,
        .alignment = peakAlignment,
        .memoryTypeBits = (1u << GetTexturesMemoryTypeIndex(bucketIndex))
    };

    VmaAllocationCreateInfo allocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    VkResult result = vmaAllocateMemory(ContextBuilder::context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.used = true;

    return true;
}

bool RenderGraph::ResizeBufferPoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
{
    if(peakSize == 0) { return true; }

    MemoryBucket& memoryBucket = buffersMemoryBucket[bucketIndex];

    if(memoryBucket.used && memoryBucket.AllocationInfo.size >= peakSize)
    {
        return true;
    }

    for(int32_t i = static_cast<int32_t>(buffersPool.size()) - 1; i >= 0; i--)
    {
        std::pair<BufferResource, bool>& buffer = buffersPool[i];

        if(buffer.first.MemoryInfo.BucketIndex != bucketIndex) { continue; }

        vkDestroyBuffer(ContextBuilder::context.Device, buffer.first.Buffer.Buffer, nullptr);

        buffersPool.erase(buffersPool.begin() + i);
    }

    if(memoryBucket.used)
    {
        vmaFreeMemory(ContextBuilder::context.Allocator, memoryBucket.Allocation);
        memoryBucket.used = false;
    }

    VkMemoryRequirements memReqs
    {
        .size = peakSize + Eve::Settings::transientBuffersStepPoolSize,
        .alignment = peakAlignment,
        .memoryTypeBits = (1u << GetBuffersMemoryTypeIndex(bucketIndex))
    };

    VmaAllocationCreateInfo allocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    VkResult result = vmaAllocateMemory(ContextBuilder::context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.used = true;

    return true;
}

TransientTextureHandle RenderGraph::RequestTransientTexture1D(TransientTextureInfo1D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();

    TextureInfo data
    {
        .Data.Width = textureInfo.Width, 
        .Data.Height = 1,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Format = textureInfo.Format,
        .Data.Usage = static_cast<TextureUsage>(0),
        .Data.Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);

    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture2D(TransientTextureInfo2D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();
    TextureInfo data
    {
        .Data.Width = textureInfo.Width, 
        .Data.Height = textureInfo.Height,
        .Data.Depth = 1,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Format = textureInfo.Format,
        .Data.Usage = static_cast<TextureUsage>(0),
        .Data.Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);
    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture3D(TransientTextureInfo3D textureInfo)
{
    TransientTextureHandle handle = MemoryManager::ReserveTransientTextureSlot();
    TextureInfo data
    {
        .Data.Width = textureInfo.Width, 
        .Data.Height = textureInfo.Height,
        .Data.Depth = textureInfo.Depth,
        .Data.ArrayLayers = textureInfo.ArrayLayers,
        .Data.MipLevels = textureInfo.MipLevels,
        .Data.Format = textureInfo.Format,
        .Data.Usage = static_cast<TextureUsage>(0),
        .Data.Sample = textureInfo.Sample
    };

    requestedTextures.push_back(data);
    return handle;
}

TransientBufferHandle RenderGraph::RequestTransientBuffer(TransientBufferInfo bufferInfo)
{
    TransientBufferHandle handle = MemoryManager::ReserveTransientBufferSlot();

    BufferInfo data
    {
        .Data.Size = bufferInfo.Size,
        .Data.Usage = static_cast<BufferUsage>(0)
    };

    requestedBuffers.push_back(data);
    return handle;
}

void RenderGraph::AddPass(GraphicsPass* pass)
{
    passes.emplace_back(
        pass->GetTextures(),
        pass->GetBuffers());
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