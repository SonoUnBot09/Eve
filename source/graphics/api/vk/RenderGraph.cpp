#include "Eve/graphics/Buffer.hpp"
#include "Eve/graphics/Texture.hpp"
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
    // TODO: store buffers and textures in texturesToReuse and buffersToReuse

    std::vector<uint32_t> freeTexturesSlot;
    std::vector<uint32_t> freeBuffersSlot;
    for(uint32_t i = 0; i < texturesToReuse.size(); i++)
    {
        std::pair<TextureResource, bool>& data = texturesToReuse[i];

        if(data.second == false) { freeTexturesSlot.push_back(i); continue; }

        if(data.first.FramesCount == 0) { freeTexturesSlot.push_back(i); continue; }

        data.first.FramesCount--;
    }
    for(uint32_t i = 0; i < buffersToReuse.size(); i++)
    {
        std::pair<BufferResource, bool>& data = buffersToReuse[i];

        if(data.second == false) { freeBuffersSlot.push_back(i); continue; }

        if(data.first.FramesCount == 0) { freeBuffersSlot.push_back(i); continue; }

        data.first.FramesCount--;
    }

    // Populate texturesToReuse and buffersToReuse with new data
    uint32_t freeTexturesCount = freeTexturesSlot.size();
    uint32_t freeBuffersCount = freeBuffersSlot.size();
    uint32_t freeTexturesOffset = 0;
    uint32_t freeBuffersOffset = 0;
    for(uint32_t i = 0; i < transientTextures[frameIndex].size(); i++)
    {
        TextureResource& data = transientTextures[frameIndex][i];
        if(data.TextureInfo.Usage == static_cast<TextureUsage>(0)) { continue; }

        if(freeTexturesOffset < freeTexturesCount)
        {
            uint32_t index = freeTexturesSlot[freeTexturesOffset];

            texturesToReuse[index] = std::pair{data, true};

            freeTexturesOffset++;
            
            continue;
        }

        texturesToReuse.push_back(std::pair{data, true});

    }
    for(uint32_t i = 0; i < transientBuffers[frameIndex].size(); i++)
    {
        BufferResource& data = transientBuffers[frameIndex][i];
        if(data.BufferInfo.Usage == static_cast<BufferUsage>(0)) { continue; }

        if(freeBuffersOffset < freeBuffersCount)
        {
            uint32_t index = freeBuffersSlot[freeBuffersOffset];

            buffersToReuse[index] = std::pair{data, true};

            freeBuffersOffset++;
            
            continue;
        }

        buffersToReuse.push_back(std::pair{data, true});

    }

    std::sort(texturesToReuse.begin(), texturesToReuse.end(), [](const std::pair<TextureResource, bool>& a, const std::pair<TextureResource, bool>& b) {
        return memcmp(&a.first.TextureInfo, &b.first.TextureInfo, sizeof(TextureInfo)) < 0;
    });
    std::sort(buffersToReuse.begin(), buffersToReuse.end(), [](const std::pair<BufferResource, bool>& a, const std::pair<BufferResource, bool>& b) {
        return memcmp(&a.first.BufferInfo, &b.first.BufferInfo, sizeof(BufferInfo)) < 0;
    });

    uint32_t texturesCount = requestedTextures.size();
    uint32_t buffersCount = requestedBuffers.size();

    textureBarriers.clear();
    bufferBarriers.clear();
    lastUsageTextures.clear();
    lastUsageBuffers.clear();
    transientTextures[frameIndex].clear();
    transientBuffers[frameIndex].clear();

    textureBarriers.resize(texturesCount);
    bufferBarriers.resize(buffersCount);

    lastUsageTextures.resize(texturesCount);
    lastUsageBuffers.resize(buffersCount);

    transientTextures[frameIndex].resize(texturesCount);
    transientBuffers[frameIndex].resize(buffersCount);

    
    // Find the first and last pass for each resource
    // Calculate the resource usage so it can be used to create/reuse a resource
    // Calculate textures and buffer barriers
    // Calculate last usage of textures and buffers
    for (uint32_t textureId = 0; textureId < texturesCount; textureId++)
    {
        TextureBarrierInfo lastUsageInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Layout = VK_IMAGE_LAYOUT_UNDEFINED
        };

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

                TextureBarrierInfo barrierInfo = CalculateTextureBarrierInfo(resource.second);

                if(IsReadOnly(resource.second))
                {
                    
                    if(barrierInfo.Layout == lastUsageInfo.Layout)
                    {
                        // Layout is the same as before no barrier needed
                        lastUsageInfo.StageMask |= barrierInfo.StageMask;
                        lastUsageInfo.AccessMask |= barrierInfo.AccessMask;

                    }
                    else 
                    {
                        // Layout is not the same as beafore barrier needed

                        TextureBarrierInfo srcBarrier = lastUsageInfo;

                        lastUsageInfo = barrierInfo;

                        TextureBarrierInfo dstBarrier
                        {
                            .StageMask = lastUsageInfo.StageMask,
                            .AccessMask = lastUsageInfo.AccessMask,
                            .Layout = lastUsageInfo.Layout
                        };

                        textureBarriers[textureId].emplace_back(
                            srcBarrier,
                            dstBarrier,
                            passIndex
                        );
                    }
                }
                else 
                {
                    TextureBarrierInfo srcBarrier = lastUsageInfo;

                    lastUsageInfo = barrierInfo;
                    TextureBarrierInfo dstBarrier = lastUsageInfo;
                    
                    textureBarriers[textureId].emplace_back(
                        srcBarrier,
                        dstBarrier,
                        passIndex
                    );

                }

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

        transientTextures[frameIndex][textureId].TextureInfo = requestedTextures[textureId];
        transientTextures[frameIndex][textureId].TextureInfo.Usage = usage;

        lastUsageTextures[textureId] = lastUsageInfo;
    }
    for (uint32_t bufferId = 0; bufferId < buffersCount; bufferId++)
    {
        BufferBarrierInfo lastUsageInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
        };

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

                BufferBarrierInfo barrierInfo = CalculateBufferBarrierInfo(resource.second);

                if(IsReadOnly(resource.second))
                {
                    lastUsageInfo.StageMask |= barrierInfo.StageMask;
                    lastUsageInfo.AccessMask |= barrierInfo.AccessMask;
                    
                }
                else 
                {
                    BufferBarrierInfo srcBarrier = lastUsageInfo;

                    lastUsageInfo = barrierInfo;
                    BufferBarrierInfo dstBarrier = lastUsageInfo;
                    
                    bufferBarriers[bufferId].emplace_back(
                        srcBarrier,
                        dstBarrier,
                        passIndex
                    );

                }

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

        transientBuffers[frameIndex][bufferId].BufferInfo = requestedBuffers[bufferId];
        transientBuffers[frameIndex][bufferId].BufferInfo.Usage = usage;

        lastUsageBuffers[bufferId] = lastUsageInfo;
    }

    textureMemorySlots.clear();
    bufferMemorySlots.clear();
    textureOccupancyCount.clear();
    bufferOccupancyCount.clear();
    usedTextureMemorySlotIds.clear();
    usedBufferMemorySlotIds.clear();
    textureVirtualAllocations.clear();
    bufferVirtualAllocations.clear();

    textureOccupancyCount.resize(texturesCount);
    usedTextureMemorySlotIds.resize(texturesCount * texturesCount);

    bufferOccupancyCount.resize(buffersCount);
    usedBufferMemorySlotIds.resize(buffersCount * buffersCount);

    textureVirtualAllocations.resize(texturesCount);
    bufferVirtualAllocations.resize(buffersCount);

    VmaVirtualBlockCreateInfo textureVirtualBlockCI
    {
        .size = UINT64_MAX
    };
    VmaVirtualBlockCreateInfo bufferVirtualBlockCI
    {
        .size = UINT64_MAX
    };

    VmaVirtualBlock textureBlock;
    VmaVirtualBlock bufferBlock;
    vmaCreateVirtualBlock(&textureVirtualBlockCI, &textureBlock);
    vmaCreateVirtualBlock(&bufferVirtualBlockCI,  &bufferBlock);

    uint64_t peakTexturesPoolSize = 0;
    uint64_t peakBuffersPoolSize = 0;
    std::vector<TransientTextureHandle> recordedTextureHandles;
    std::vector<TransientBufferHandle> recordedBufferHandles;
    for (uint32_t passIndex = 0; passIndex < passes.size(); passIndex++)
    {
        Pass& pass = passes[passIndex];

        // Texture Creation
        for(uint32_t i = 0; i < pass.TexturesToCreate.size(); i++)
        {
            TransientTextureHandle handle = pass.TexturesToCreate[i];
            if(transientTextures[frameIndex][handle.Id].TextureInfo.Usage == static_cast<TextureUsage>(0)) { continue; }

            TextureInfo textureInfo = transientTextures[frameIndex][handle.Id].TextureInfo;

            VkImageCreateInfo virtualImageCI = RenderGraph::GetVirtualTextureCreateInfo(textureInfo);

            VkDeviceImageMemoryRequirements virtualMemoryInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_IMAGE_MEMORY_REQUIREMENTS,
                .pCreateInfo = &virtualImageCI
            };

            VkMemoryRequirements2 memoryRequirements;
            vkGetDeviceImageMemoryRequirements(ContextBuilder::context.Device, &virtualMemoryInfo, &memoryRequirements);

            VmaVirtualAllocationCreateInfo allocInfo
            {
                .size = memoryRequirements.memoryRequirements.size,
                .alignment = memoryRequirements.memoryRequirements.alignment
            };

            VmaVirtualAllocation allocation;
            VkDeviceSize offset;
            vmaVirtualAllocate(textureBlock, &allocInfo, &allocation, &offset);

            uint64_t start = offset;
            uint64_t end = offset + memoryRequirements.memoryRequirements.size;
            peakTexturesPoolSize = std::max(peakTexturesPoolSize, static_cast<uint64_t>(offset) + static_cast<uint64_t>(memoryRequirements.memoryRequirements.size));

            MemorySlot currentMemorySlot{start, end, handle.Id};
            auto it = std::lower_bound(textureMemorySlots.begin(), textureMemorySlots.end(), currentMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
                return a.End <= b.Start;
            });

            uint32_t slotOffset = 0;
            uint32_t slotsCount = 0;
            int32_t firstSlotIndex = it - textureMemorySlots.begin();
            for(; it != textureMemorySlots.end(); it++)
            {
                if(it->Start >= currentMemorySlot.End) { break; }

                slotsCount++;

                bool alreadyRecorded = false;
                for(uint32_t i = 0; i < recordedTextureHandles.size(); i++)
                {
                    if(recordedTextureHandles[i].Id == it->ResourceId) { alreadyRecorded = true; break; }
                }

                if(alreadyRecorded) { continue; }

                recordedTextureHandles.emplace_back(it->ResourceId);
                usedTextureMemorySlotIds[texturesCount * handle.Id + slotOffset] = it->ResourceId;
                slotOffset++;
            }
            textureOccupancyCount[handle.Id] = slotOffset;
            recordedTextureHandles.clear();

            if(slotsCount > 0)
            {
                int32_t lastSlotIndex = firstSlotIndex + slotsCount - 1;
                MemorySlot firstSlot = textureMemorySlots[firstSlotIndex];
                MemorySlot lastSlot = textureMemorySlots[lastSlotIndex];

                int64_t strideStart = currentMemorySlot.Start - firstSlot.Start;
                int64_t strideEnd = lastSlot.End - currentMemorySlot.End;

                MemorySlot fragments[3];
                uint32_t fragmentsCount = 0;

                if(strideStart > 0)
                {
                    // Free memory at the beginnig of the slot
                    MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceId};

                    fragments[fragmentsCount++] = startSlot;
                }
                
                fragments[fragmentsCount++] = currentMemorySlot;

                if(strideEnd > 0)
                {
                    // Free memory at the end of the slot
                    MemorySlot endSlot{currentMemorySlot.End, currentMemorySlot.End + strideEnd, lastSlot.ResourceId};

                    fragments[fragmentsCount++] = endSlot;
                }

                textureMemorySlots.erase(
                    textureMemorySlots.begin() + firstSlotIndex, 
                    textureMemorySlots.begin() + lastSlotIndex + 1);

                textureMemorySlots.insert(textureMemorySlots.begin() + firstSlotIndex, fragments, fragments + fragmentsCount);

            }
            else 
            {
                textureMemorySlots.insert(textureMemorySlots.begin() + firstSlotIndex, currentMemorySlot);
            }


            transientTextures[frameIndex][handle.Id].TextureInfo.Offset = offset;
            transientTextures[frameIndex][handle.Id].TextureInfo.Stride = memoryRequirements.memoryRequirements.size;

            textureVirtualAllocations[handle.Id] = allocation;
        }

        // Buffer Creation
        for(uint32_t i = 0; i < pass.BuffersToCreate.size(); i++)
        {
            TransientBufferHandle handle = pass.BuffersToCreate[i];
            if(transientBuffers[frameIndex][handle.Id].BufferInfo.Usage == static_cast<BufferUsage>(0)) { continue; }

            BufferInfo bufferInfo = transientBuffers[frameIndex][handle.Id].BufferInfo;

            VkBufferCreateInfo virtualBufferCI = RenderGraph::GetVirtualBufferCreateInfo(bufferInfo);

            VkDeviceBufferMemoryRequirements virtualMemoryInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
                .pCreateInfo = &virtualBufferCI
            };

            VkMemoryRequirements2 memoryRequirements;
            vkGetDeviceBufferMemoryRequirements(ContextBuilder::context.Device, &virtualMemoryInfo, &memoryRequirements);

            VmaVirtualAllocationCreateInfo allocInfo
            {
                .size = memoryRequirements.memoryRequirements.size,
                .alignment = memoryRequirements.memoryRequirements.alignment
            };

            VmaVirtualAllocation allocation;
            VkDeviceSize offset;
            vmaVirtualAllocate(bufferBlock, &allocInfo, &allocation, &offset);

            uint64_t start = offset;
            uint64_t end = offset + memoryRequirements.memoryRequirements.size;
            peakBuffersPoolSize = std::max(peakBuffersPoolSize, static_cast<uint64_t>(offset) + static_cast<uint64_t>(memoryRequirements.memoryRequirements.size));

            MemorySlot currentMemorySlot{start, end, handle.Id};
            auto it = std::lower_bound(bufferMemorySlots.begin(), bufferMemorySlots.end(), currentMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
                return a.End <= b.Start;
            });

            uint32_t slotOffset = 0;
            uint32_t slotsCount = 0;
            int32_t firstSlotIndex = it - bufferMemorySlots.begin();
            for(; it != bufferMemorySlots.end(); it++)
            {
                if(it->Start >= currentMemorySlot.End) { break; }

                slotsCount++;

                bool alreadyRecorded = false;
                for(uint32_t i = 0; i < recordedBufferHandles.size(); i++)
                {
                    if(recordedBufferHandles[i].Id == it->ResourceId) { alreadyRecorded = true; break; }
                }

                if(alreadyRecorded) { continue; }

                recordedBufferHandles.emplace_back(it->ResourceId);
                usedBufferMemorySlotIds[buffersCount * handle.Id + slotOffset] = it->ResourceId;
                slotOffset++;
            }
            bufferOccupancyCount[handle.Id] = slotOffset;
            recordedBufferHandles.clear();

            if(slotsCount > 0)
            {
                int32_t lastSlotIndex = firstSlotIndex + slotsCount - 1;
                MemorySlot firstSlot = bufferMemorySlots[firstSlotIndex];
                MemorySlot lastSlot = bufferMemorySlots[lastSlotIndex];

                int64_t strideStart = currentMemorySlot.Start - firstSlot.Start;
                int64_t strideEnd = lastSlot.End - currentMemorySlot.End;

                MemorySlot fragments[3];
                uint32_t fragmentsCount = 0;

                if(strideStart > 0)
                {
                    // Free memory at the beginnig of the slot
                    MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceId};

                    fragments[fragmentsCount++] = startSlot;
                }
                
                fragments[fragmentsCount++] = currentMemorySlot;

                if(strideEnd > 0)
                {
                    // Free memory at the end of the slot
                    MemorySlot endSlot{currentMemorySlot.End, currentMemorySlot.End + strideEnd, lastSlot.ResourceId};

                    fragments[fragmentsCount++] = endSlot;
                }

                bufferMemorySlots.erase(
                    bufferMemorySlots.begin() + firstSlotIndex, 
                    bufferMemorySlots.begin() + lastSlotIndex + 1);

                bufferMemorySlots.insert(bufferMemorySlots.begin() + firstSlotIndex, fragments, fragments + fragmentsCount);

            }
            else 
            {
                bufferMemorySlots.insert(bufferMemorySlots.begin() + firstSlotIndex, currentMemorySlot);
            }


            transientBuffers[frameIndex][handle.Id].BufferInfo.Offset = offset;
            transientBuffers[frameIndex][handle.Id].BufferInfo.Stride = memoryRequirements.memoryRequirements.size;

            bufferVirtualAllocations[handle.Id] = allocation;
        }

        // Texture Destruction
        for(uint32_t i = 0; i < pass.TexturesToDestroy.size(); i++)
        {
            TransientTextureHandle handle = pass.TexturesToDestroy[i];
            if(transientTextures[frameIndex][handle.Id].TextureInfo.Usage == static_cast<TextureUsage>(0)) { continue; }

            VmaVirtualAllocation virtualAllocation = textureVirtualAllocations[handle.Id];

            vmaVirtualFree(textureBlock, virtualAllocation);
        }

        // Buffer Destruction
        for(uint32_t i = 0; i < pass.BuffersToDestroy.size(); i++)
        {
            TransientBufferHandle handle = pass.BuffersToDestroy[i];
            if(transientBuffers[frameIndex][handle.Id].BufferInfo.Usage == static_cast<BufferUsage>(0)) { continue; }

            VmaVirtualAllocation virtualAllocation = bufferVirtualAllocations[handle.Id];

            vmaVirtualFree(bufferBlock, virtualAllocation);
        }

    }

    vmaDestroyVirtualBlock(textureBlock);
    vmaDestroyVirtualBlock(bufferBlock);

    // TODO: Create new pools if the real allocated memory is not enough and destroy the old pools

    
    //Find or create new textures/buffers to use
    for (uint32_t textureId = 0; textureId < requestedTextures.size(); textureId++)
    {
        TextureInfo textureInfo = transientTextures[frameIndex][textureId].TextureInfo;

        if(textureInfo.Usage == static_cast<TextureUsage>(0)) { continue; }

        auto it = std::lower_bound(texturesToReuse.begin(), texturesToReuse.end(), textureInfo,[](const std::pair<TextureResource, bool>& a, const TextureInfo& b)
        {
            return std::memcmp(&a.first.TextureInfo, &b, sizeof(TextureInfo)) < 0;
        });

        bool found = false;
        for(; it != texturesToReuse.end(); it++)
        {
            if(std::memcmp(&it->first.TextureInfo, &textureInfo, sizeof(TextureInfo)) != 0)
            {
                break;
            }

            if(it->second == false)
            {
                found = true;
                it->second = true;
                break;
            }
        }
        
        Texture texture;
        if(found)
        {
            texture = it->first.Texture;
        }
        else 
        {
            texture = AllocateTransientTexture(textureInfo);

        }

        transientTextures[frameIndex][textureId].Texture = texture;

    }
    for (uint32_t bufferId = 0; bufferId < requestedBuffers.size(); bufferId++)
    {
        BufferInfo bufferInfo = transientBuffers[frameIndex][bufferId].BufferInfo;

        if(bufferInfo.Usage == static_cast<BufferUsage>(0)) { continue; }

        auto it = std::lower_bound(buffersToReuse.begin(), buffersToReuse.end(), bufferInfo,[](const std::pair<BufferResource, bool>& a, const BufferInfo& b)
        {
            return std::memcmp(&a.first.BufferInfo, &b, sizeof(BufferInfo)) < 0;
        });

        bool found = false;
        for(; it != buffersToReuse.end(); it++)
        {
            if(std::memcmp(&it->first.BufferInfo, &bufferInfo, sizeof(BufferInfo)) != 0)
            {
                break;
            }

            if(it->second == false)
            {
                found = true;
                it->second = true;
                break;
            }
        }

        Buffer buffer;
        if(found)
        {
            buffer = it->first.Buffer;
        }
        else 
        {
            buffer = AllocateTransientBuffer(bufferInfo);

        }

        transientBuffers[frameIndex][bufferId].Buffer = buffer;

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

                bool isReadOnly = IsReadOnly(resource.second);
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
                

                bool isReadOnly = IsReadOnly(resource.second);
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

bool RenderGraph::IsReadOnly(Usage usage)
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

Texture RenderGraph::AllocateTransientTexture(TextureInfo textureInfo)
{
    Texture image;

    VkImageType imageType;
    VkImageViewType imageViewType;
    if(textureInfo.Width > 1 && textureInfo.Height > 1 && textureInfo.Depth > 1)
    {
        imageType = VK_IMAGE_TYPE_3D;
        imageViewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if(textureInfo.Width > 1 && textureInfo.Height > 1)
    {
        imageType = VK_IMAGE_TYPE_2D;
        imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    }
    else
    {
        imageType = VK_IMAGE_TYPE_1D;
        imageViewType = VK_IMAGE_VIEW_TYPE_1D;
    }

    VkFormat format = GetVkImageFormat(textureInfo.Format);
    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = imageType,
        .format = format,
        .extent {.width = textureInfo.Width, .height = 1, .depth = 1},
        .mipLevels = textureInfo.MipLevels,
        .arrayLayers = textureInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(textureInfo.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(textureInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo imageAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateImage(ContextBuilder::context.Allocator, &imageCI, &imageAllocInfo, 
        &image.Image, &image.Allocation, &image.AllocationInfo);
    
    VkImageViewCreateInfo imageViewCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.Image,
        .viewType = imageViewType,
        .format = format,
        .subresourceRange
        {
            .aspectMask = GetVkImageAspectMaskBasedOnFormat(format),
            .baseMipLevel = 0,
            .levelCount = textureInfo.MipLevels,
            .baseArrayLayer = 0,
            .layerCount = textureInfo.ArrayLayers
        }
    };

    vkCreateImageView(ContextBuilder::context.Device, &imageViewCI, nullptr, &image.ImageView);
   
    return image;
}

Buffer RenderGraph::AllocateTransientBuffer(BufferInfo bufferInfo)
{
    Buffer buffer;

    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferInfo.Size,
        .usage = GetVkBufferUsage(bufferInfo.Usage) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    VmaAllocationCreateInfo bufferAllocInfo
    {
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
    };

    vmaCreateBuffer(ContextBuilder::context.Allocator, &bufferCI, &bufferAllocInfo,
         &buffer.Buffer, &buffer.Allocation, &buffer.AllocationInfo);

    return buffer;
}

VkImageCreateInfo RenderGraph::GetVirtualTextureCreateInfo(TextureInfo textureInfo)
{

    VkImageType imageType;
    if(textureInfo.Width > 1 && textureInfo.Height > 1 && textureInfo.Depth > 1)
    {
        imageType = VK_IMAGE_TYPE_3D;
    }
    else if(textureInfo.Width > 1 && textureInfo.Height > 1)
    {
        imageType = VK_IMAGE_TYPE_2D;
    }
    else
    {
        imageType = VK_IMAGE_TYPE_1D;
    }

    VkImageCreateInfo imageCI
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = imageType,
        .format = GetVkImageFormat(textureInfo.Format),
        .extent {.width = textureInfo.Width, .height = 1, .depth = 1},
        .mipLevels = textureInfo.MipLevels,
        .arrayLayers = textureInfo.ArrayLayers,
        .samples = GetVkImageSamplesCount(textureInfo.Sample),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = GetVkImageUsage(textureInfo.Usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    return imageCI;

};

VkBufferCreateInfo RenderGraph::GetVirtualBufferCreateInfo(BufferInfo bufferInfo)
{
    VkBufferCreateInfo bufferCI
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bufferInfo.Size,
        .usage = GetVkBufferUsage(bufferInfo.Usage) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    return bufferCI;
}