#include <graphics/ResourceMapper.hpp>
#include "EveSettings.hpp"
#include "Resources.hpp"
#include <graphics/helpers/VulkanMapping.hpp>

using namespace Eve::Graphics;

void ResourceMapper::CreateGlobalDescriptor(uint32_t maxImagesCount, uint32_t maxSamplersCount, uint32_t maxBuffersCount)
{
    #pragma region Layout

        std::vector<VkDescriptorSetLayoutBinding> bindingsLayout
        {
            // 1D Textures
            VkDescriptorSetLayoutBinding
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // 2D Textures
            VkDescriptorSetLayoutBinding
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // 3D Textures
            VkDescriptorSetLayoutBinding
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // Cubemaps
            VkDescriptorSetLayoutBinding
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // Samplers
            VkDescriptorSetLayoutBinding
            {
                .binding = 3,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = maxSamplersCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
            },

            // Buffers
            VkDescriptorSetLayoutBinding
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            }
        };

        std::vector<VkDescriptorBindingFlags> bindingFlags(6);
        bindingFlags[0] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[2] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[3] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[4] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[5] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = static_cast<uint32_t>(bindingFlags.size()),
            .pBindingFlags = bindingFlags.data()
        };
        
        VkDescriptorSetLayoutCreateInfo layoutCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &flagsCI,
            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
            .bindingCount = static_cast<uint32_t>(bindingsLayout.size()),
            .pBindings = bindingsLayout.data()
        };

        vkCreateDescriptorSetLayout(GraphicsCore::Context.Device, &layoutCI, nullptr, &layout);

    #pragma endregion

    #pragma region Pool

        std::vector<VkDescriptorPoolSize> poolSize
        {
            // Texture 1D
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture 2D
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture 3D
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture Cubemap
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Samplers
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = maxSamplersCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Buffers
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1 * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },
        };

        VkDescriptorPoolCreateInfo poolCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = Eve::Settings::MAX_FRAMES_IN_FLIGHT,
            .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
            .pPoolSizes = poolSize.data()
        };

        vkCreateDescriptorPool(GraphicsCore::Context.Device, &poolCI, nullptr, &pool);
    
    #pragma endregion

    #pragma region Set

        std::vector<VkDescriptorSetLayout> layouts(Eve::Settings::MAX_FRAMES_IN_FLIGHT, layout);

        VkDescriptorSetAllocateInfo setAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = Eve::Settings::MAX_FRAMES_IN_FLIGHT,
            .pSetLayouts = layouts.data()
        };

        vkAllocateDescriptorSets(GraphicsCore::Context.Device, &setAllocInfo, sets.data());

        std::vector<VkWriteDescriptorSet> descriptorWrites;
        descriptorWrites.reserve(Eve::Settings::MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
        {
            BufferInfo bufferInfo
            {
                .Data.Size = maxBuffersCount * sizeof(uint64_t),
                .Data.Usage = BufferUsage::BUFFER_USAGE_STORAGE | BufferUsage::BUFFER_USAGE_TRANSFER_DST
            };

            BufferHandle handle = MemoryRegistry::CreateGPUBuffer(bufferInfo);

            BDABuffers.push_back(handle);

            VkDescriptorBufferInfo bufferWriteInfo
            {
                .buffer = MemoryRegistry::GetBuffer(handle).Buffer,
                .offset = 0,
                .range = maxBuffersCount * sizeof(uint64_t)
            };

            VkWriteDescriptorSet writeInfo
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = sets[i],
                .dstBinding = 5,
                .dstArrayElement = 0,
                .descriptorCount = 1,
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

            BufferHandle handle = MemoryRegistry::CreateCPUBuffer(bufferInfo);

            stagingBufferHandles.push_back(handle);

            BufferObject& buffer = MemoryRegistry::GetBuffer(handle);

            stagingBuffers.push_back(buffer);
        }
        
    #pragma endregion
}

void ResourceMapper::DestroyGlobalDescriptor()
{
    if(layout)
    {
        vkDestroyDescriptorSetLayout(GraphicsCore::Context.Device, layout, nullptr);
    }

    if(pool)
    {
        vkDestroyDescriptorPool(GraphicsCore::Context.Device, pool, nullptr);
    }
}

bool ResourceMapper::MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex)
{
    descriptorSetWrites.clear();
    buffersAddress.clear();
    copyRegions.clear();

    // Images erase
    uint32_t images1DToErase = 0;
    for(int32_t i = images1DToMap.size() - 1; i >= 0; i--)
    {
        if(images1DToMap[i].second <= 0)
        {
            images1DToErase++;
        }
        else 
        {
            break;
        }
    }

    uint32_t images2DToErase = 0;
    for(int32_t i = images2DToMap.size() - 1; i >= 0; i--)
    {
        if(images2DToMap[i].second <= 0)
        {
            images2DToErase++;
        }
        else 
        {
            break;
        }
    }

    uint32_t images3DToErase = 0;
    for(int32_t i = images3DToMap.size() - 1; i >= 0; i--)
    {
        if(images3DToMap[i].second <= 0)
        {
            images3DToErase++;
        }
        else 
        {
            break;
        }
    }

    uint32_t imagesCubeToErase = 0;
    for(int32_t i = imagesCubeToMap.size() - 1; i >= 0; i--)
    {
        if(imagesCubeToMap[i].second <= 0)
        {
            imagesCubeToErase++;
        }
        else 
        {
            break;
        }
    }

    // Sampler erase
    uint32_t samplersToErase = 0;
    for(int32_t i = samplersToMap.size() - 1; i >= 0; i--)
    {
        if(samplersToMap[i].second <= 0)
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
    for(int32_t i = buffersToMap.size() - 1; i >= 0; i--)
    {
        if(buffersToMap[i].second <= 0)
        {
            buffersToErase++;
        }
        else 
        {
            break;
        }
    }

    // Erase
    images1DToMap.erase(images1DToMap.cend() - images1DToErase, images1DToMap.cend());
    images2DToMap.erase(images2DToMap.cend() - images2DToErase, images2DToMap.cend());
    images3DToMap.erase(images3DToMap.cend() - images3DToErase, images3DToMap.cend());
    imagesCubeToMap.erase(imagesCubeToMap.cend() - imagesCubeToErase, imagesCubeToMap.cend());
    samplersToMap.erase(samplersToMap.cend() - samplersToErase, samplersToMap.cend());
    buffersToMap.erase(buffersToMap.cend() - buffersToErase, buffersToMap.cend());


    std::vector<VkDescriptorImageInfo> images1DInfo;
    std::vector<VkDescriptorImageInfo> images2DInfo;
    std::vector<VkDescriptorImageInfo> images3DInfo;
    std::vector<VkDescriptorImageInfo> imagesCubeInfo;
    std::vector<VkDescriptorImageInfo> samplersInfo;

    images1DInfo.reserve(images1DToMap.size());
    images1DInfo.reserve(images2DToMap.size());
    images1DInfo.reserve(images3DToMap.size());
    images1DInfo.reserve(imagesCubeToMap.size());
    samplersInfo.reserve(samplersToMap.size());

    // Images
    for(uint32_t i = 0; i < images1DToMap.size(); i++)
    {
        uint32_t framesToLive = images1DToMap[i].second;

        if(framesToLive == 0) { continue; }

        images1DToMap[i].second--;

        TextureHandle handle = images1DToMap[i].first;

        TextureObject& image = MemoryRegistry::GetTexture(handle);

        VkDescriptorImageInfo imageInfo
        {
            .imageView = image.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .sampler = VK_NULL_HANDLE
        };

        images1DInfo.push_back(imageInfo);

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

    for(uint32_t i = 0; i < images2DToMap.size(); i++)
    {
        uint32_t framesToLive = images2DToMap[i].second;

        if(framesToLive == 0) { continue; }

        images2DToMap[i].second--;

        TextureHandle handle = images2DToMap[i].first;

        TextureObject& image = MemoryRegistry::GetTexture(handle);

        VkDescriptorImageInfo imageInfo
        {
            .imageView = image.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .sampler = VK_NULL_HANDLE
        };

        images1DInfo.push_back(imageInfo);

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

    for(uint32_t i = 0; i < images3DToMap.size(); i++)
    {
        uint32_t framesToLive = images3DToMap[i].second;

        if(framesToLive == 0) { continue; }

        images3DToMap[i].second--;

        TextureHandle handle = images3DToMap[i].first;

        TextureObject& image = MemoryRegistry::GetTexture(handle);

        VkDescriptorImageInfo imageInfo
        {
            .imageView = image.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .sampler = VK_NULL_HANDLE
        };

        images1DInfo.push_back(imageInfo);

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

    for(uint32_t i = 0; i < imagesCubeToMap.size(); i++)
    {
        uint32_t framesToLive = imagesCubeToMap[i].second;

        if(framesToLive == 0) { continue; }

        imagesCubeToMap[i].second--;

        TextureHandle handle = imagesCubeToMap[i].first;

        TextureObject& image = MemoryRegistry::GetTexture(handle);

        VkDescriptorImageInfo imageInfo
        {
            .imageView = image.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
            .sampler = VK_NULL_HANDLE
        };

        images1DInfo.push_back(imageInfo);

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

        SamplerObject sampler = MemoryRegistry::GetSampler(handle);

        VkDescriptorImageInfo imageInfo
        {
            .sampler = sampler.Sampler,
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        samplersInfo.push_back(imageInfo);

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

        BufferObject& buffer = MemoryRegistry::GetBuffer(handle);

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

    memcpy(stagingBuffers[frameIndex].AllocationInfo.pMappedData, buffersAddress.data(), buffersAddress.size() * sizeof(uint64_t));

    vkCmdCopyBuffer
    (
        cmdBuffer, 

        stagingBuffers[frameIndex].Buffer,
        MemoryRegistry::GetBuffer(BDABuffers[frameIndex]).Buffer,

        copyRegions.size(),
        copyRegions.data()
    );

    VkBufferMemoryBarrier2 barrier
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
        .buffer = MemoryRegistry::GetBuffer(BDABuffers[frameIndex]).Buffer,
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

void ResourceMapper::ScheduleImageMapping1D(TextureHandle handle)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    if(image1DToMapFreeSlots.empty())
    {
        images1DToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = image1DToMapFreeSlots.back();
        image1DToMapFreeSlots.pop_back();

        images1DToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}

void ResourceMapper::ScheduleImageMapping2D(TextureHandle handle)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    if(image2DToMapFreeSlots.empty())
    {
        images2DToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = image2DToMapFreeSlots.back();
        image2DToMapFreeSlots.pop_back();

        images2DToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}

void ResourceMapper::ScheduleImageMapping3D(TextureHandle handle)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    if(image3DToMapFreeSlots.empty())
    {
        images3DToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = image3DToMapFreeSlots.back();
        image3DToMapFreeSlots.pop_back();

        images3DToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
    }
}

void ResourceMapper::ScheduleImageMappingCube(TextureHandle handle)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    if(imageCubeToMapFreeSlots.empty())
    {
        imagesCubeToMap.push_back({handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT});
    }
    else 
    {
        uint32_t index = imageCubeToMapFreeSlots.back();
        imageCubeToMapFreeSlots.pop_back();

        imagesCubeToMap[index] = {handle, Eve::Settings::MAX_FRAMES_IN_FLIGHT};
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
