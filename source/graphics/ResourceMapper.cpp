#include <graphics/ResourceMapper.hpp>
#include <graphics/VulkanMapping.hpp>
#include <graphics/MemoryManager.hpp>

using namespace Eve::Graphics;

void ResourceMapper::CreateGlobalDescriptor(uint32_t maxImagesCount, uint32_t maxSamplersCount, uint32_t maxBuffersCount)
{
    #pragma region Layout

        std::vector<VkDescriptorSetLayoutBinding> bindingsLayout;
        VkDescriptorSetLayoutBinding imagesDescriptor
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = maxImagesCount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        VkDescriptorSetLayoutBinding samplersDescriptor
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = maxSamplersCount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        VkDescriptorSetLayoutBinding buffersDescriptor
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = maxBuffersCount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
        };

        bindingsLayout.push_back(imagesDescriptor);
        bindingsLayout.push_back(samplersDescriptor);
        bindingsLayout.push_back(buffersDescriptor);
        
        VkDescriptorSetLayoutCreateInfo layoutCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindingsLayout.size()),
            .pBindings = bindingsLayout.data()
        };

        vkCreateDescriptorSetLayout(GraphicsCore::Context.Device, &layoutCI, nullptr, &layout);

    #pragma endregion

    #pragma region Pool

        std::vector<VkDescriptorPoolSize> poolSize;

        VkDescriptorPoolSize imageDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = maxImagesCount
        };

        VkDescriptorPoolSize samplerDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = maxSamplersCount
        };

        VkDescriptorPoolSize bufferDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1
        };

        poolSize.push_back(imageDescriptorSize);
        poolSize.push_back(samplerDescriptorSize);
        poolSize.push_back(bufferDescriptorSize);

        VkDescriptorPoolCreateInfo poolCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = Eve::Settings::MAX_FRAMES_IN_FLIGHT,
            .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
            .pPoolSizes = poolSize.data()
        };

        vkCreateDescriptorPool(GraphicsCore::Context.Device, &poolCI, nullptr, &pool);
    
    #pragma endregion

    #pragma region Set

        VkDescriptorSetAllocateInfo setAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = Eve::Settings::MAX_FRAMES_IN_FLIGHT,
            .pSetLayouts = &layout
        };

        vkAllocateDescriptorSets(GraphicsCore::Context.Device, &setAllocInfo, sets.data());

        std::vector<VkWriteDescriptorSet> descriptorWrites {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
        for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
        {
            BufferInfo bufferInfo
            {
                .Data.Size = maxBuffersCount * sizeof(uint64_t),
                .Data.Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle handle = MemoryManager::AllocateBuffer(bufferInfo);

            BDABuffers.push_back(handle);

            VkDescriptorBufferInfo bufferWriteInfo
            {
                .buffer = MemoryManager::GetBuffer(handle).Buffer,
                .offset = 0,
                .range = maxBuffersCount * sizeof(uint64_t)
            };

            VkWriteDescriptorSet writeInfo
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = sets[i],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &bufferWriteInfo
            };

            descriptorWrites.push_back(writeInfo);
        }

        vkUpdateDescriptorSets(GraphicsCore::Context.Device, descriptorWrites.size(), 
        descriptorWrites.data(), 0, nullptr);

    #pragma endregion

    #pragma region Staging Buffers
        
        for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
        {
            BufferInfo bufferInfo
            {
                .Data.Size = maxBuffersCount * sizeof(uint64_t),
                .Data.Usage = BufferUsage::BUFFER_USAGE_TRANSFER_SRC
            };

            BufferHandle handle = MemoryManager::AllocateHostBuffer(bufferInfo);

            stagingBufferHandles.push_back(handle);

            Buffer& buffer = MemoryManager::GetBuffer(handle);

            stagingBuffers.push_back(buffer);
        }
        
    #pragma endregion
}

bool ResourceMapper::MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex)
{
    descriptorSetWrites.clear();
    buffersAddress.clear();
    copyRegions.clear();

    // Images erase
    uint32_t imagesToErase = 0;
    for(uint32_t i = imagesToMap.size() - 1; i >= 0; i--)
    {
        if(imagesToMap[i].second > 0)
        {
            imagesToErase++;
        }
        else 
        {
            break;
        }
    }

    // Sampler erase
    uint32_t samplersToErase = 0;
    for(uint32_t i = samplersToMap.size() - 1; i >= 0; i--)
    {
        if(samplersToMap[i].second > 0)
        {
            samplersToErase++;
        }
        else 
        {
            break;
        }
    }

    // Buffer erase
    uint32_t buffersToErase = 0;
    for(uint32_t i = buffersToMap.size() - 1; i >= 0; i--)
    {
        if(buffersToMap[i].second > 0)
        {
            buffersToErase++;
        }
        else 
        {
            break;
        }
    }

    // Erase
    imagesToMap.erase(imagesToMap.cend() - imagesToErase, imagesToMap.cend());
    samplersToMap.erase(samplersToMap.cend() - samplersToErase, samplersToMap.cend());
    buffersToMap.erase(buffersToMap.cend() - buffersToErase, buffersToMap.cend());

    // Images
    for(uint32_t i = 0; i < imagesToMap.size(); i++)
    {
        uint32_t framesToLive = imagesToMap[i].second;

        if(framesToLive == 0) { continue; }

        imagesToMap[i].second--;

        TextureHandle handle = imagesToMap[i].first;

        Texture& image = MemoryManager::GetTexture(handle);

        VkDescriptorImageInfo imageInfo
        {
            .imageView = image.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .sampler = VK_NULL_HANDLE
        };

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 0,
            .dstArrayElement = handle.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imageInfo
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // Samplers
    for(uint32_t i = 0; i < samplersToMap.size(); i++)
    {
        uint32_t framesToLive = samplersToMap[i].second;

        if(framesToLive == 0) { continue; }

        samplersToMap[i].second--;

        SamplerHandle handle = samplersToMap[i].first;

        Sampler sampler = MemoryManager::GetSampler(handle);

        VkDescriptorImageInfo imageInfo
        {
            .sampler = sampler.Sampler,
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 1,
            .dstArrayElement = handle.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &imageInfo
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // Buffers
    for (uint32_t i = 0; i < buffersToMap.size(); i++)
    {
        uint32_t framesToLive = buffersToMap[i].second;

        if(framesToLive == 0) { continue; }

        buffersToMap[i].second--;

        BufferHandle handle = buffersToMap[i].first;

        Buffer& buffer = MemoryManager::GetBuffer(handle);

        VkBufferDeviceAddressInfo addressInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = buffer.Buffer
        };

        VkDeviceAddress address = vkGetBufferDeviceAddress(GraphicsCore::Context.Device, &addressInfo);

        buffersAddress.push_back(address);

        VkBufferCopy region
        {
            .srcOffset = i * sizeof(uint64_t),
            .dstOffset = handle.Id * sizeof(uint64_t),
            .size = sizeof(uint64_t)
        };

        copyRegions.push_back(region);
    }

    if(!descriptorSetWrites.empty())
    {
        vkUpdateDescriptorSets(GraphicsCore::Context.Device, descriptorSetWrites.size(), 
        descriptorSetWrites.data(), 0, nullptr);
    }

    if(buffersAddress.empty())
    {
        // No need to wait for a closer stage in the timeline semaphore because we do not need to wait that the copy command is finished
        // because there is any copy command since there are no buffers that need to be updated in the descriptor set
        return false; 
    }

    memcpy(stagingBuffers[frameIndex].AllocationInfo.pMappedData, buffersAddress.data(), buffersAddress.size());

    vkCmdCopyBuffer
    (
        cmdBuffer, 

        stagingBuffers[frameIndex].Buffer,
        MemoryManager::GetBuffer(BDABuffers[frameIndex]).Buffer,

        copyRegions.size(),
        copyRegions.data()
    );

    VkBufferMemoryBarrier2 barrier
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .buffer = MemoryManager::GetBuffer(BDABuffers[frameIndex]).Buffer,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .offset = 0,
        .size = VK_WHOLE_SIZE
    };

    VkDependencyInfo dependencyInfo
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .bufferMemoryBarrierCount = 1,
        .pBufferMemoryBarriers = &barrier
    };

    vkCmdPipelineBarrier2(cmdBuffer, &dependencyInfo);

    // Need to wait for a closer stage in the timeline semaphore because we need to wait that the copy command is finished
    return true;
}

void ResourceMapper::ScheduleImageMapping(TextureHandle handle)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    if(imageToMapFreeSlots.empty())
    {
        imagesToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = imageToMapFreeSlots.back();
        imageToMapFreeSlots.pop_back();

        imagesToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}

void ResourceMapper::ScheduleSamplerMapping(SamplerHandle handle)
{
    std::lock_guard<std::mutex> lock(samplersMutex);

    if(samplerToMapFreeSlots.empty())
    {
        samplersToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = samplerToMapFreeSlots.back();
        samplerToMapFreeSlots.pop_back();
        
        samplersToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}

void ResourceMapper::ScheduleBufferMapping(BufferHandle handle)
{
    std::lock_guard<std::mutex> lock(buffersMutex);

    if(bufferToMapFreeSlots.empty())
    {
        buffersToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = bufferToMapFreeSlots.back();
        bufferToMapFreeSlots.pop_back();
        
        buffersToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}
