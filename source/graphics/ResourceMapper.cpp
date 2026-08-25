#include <graphics/ResourceMapper.hpp>
#include "EveSettings.hpp"
#include "Resources.hpp"
#include "graphics/registers/MemoryRegistry.hpp"
#include <graphics/helpers/VulkanMapping.hpp>

using namespace Eve::Graphics;

namespace 
{
    bool IsSampled(VkImageUsageFlags usage)
    {
        if((usage & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            return true;
        }

        return false;
    }

    bool IsStorage(VkImageUsageFlags usage)
    {
        if((usage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
        {
            return true;
        }

        return false;
    }
}

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
            VkDescriptorSetLayoutBinding
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // 2D Textures
            VkDescriptorSetLayoutBinding
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },
            VkDescriptorSetLayoutBinding
            {
                .binding = 3,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // 3D Textures
            VkDescriptorSetLayoutBinding
            {
                .binding = 4,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },
            VkDescriptorSetLayoutBinding
            {
                .binding = 5,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // Cubemaps
            VkDescriptorSetLayoutBinding
            {
                .binding = 6,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },
            VkDescriptorSetLayoutBinding
            {
                .binding = 7,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // Samplers
            VkDescriptorSetLayoutBinding
            {
                .binding = 8,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = maxSamplersCount,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            },

            // Buffers
            VkDescriptorSetLayoutBinding
            {
                .binding = 9,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT
            }
        };

        std::vector<VkDescriptorBindingFlags> bindingFlags(10);
        bindingFlags[0] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[1] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[2] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[3] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[4] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[5] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[6] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[7] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[8] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        bindingFlags[9] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

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
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture 2D
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture 3D
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },

            // Texture Cubemap
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = maxImagesCount * Eve::Settings::MAX_FRAMES_IN_FLIGHT
            },
            VkDescriptorPoolSize
            {
                .type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
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

        std::vector<VkDescriptorBufferInfo> bufferWritesInfo;
        std::vector<VkWriteDescriptorSet> descriptorWrites;
        bufferWritesInfo.reserve(Eve::Settings::MAX_FRAMES_IN_FLIGHT * 2);
        descriptorWrites.reserve(Eve::Settings::MAX_FRAMES_IN_FLIGHT * 2);

        for (uint32_t i = 0; i < Eve::Settings::MAX_FRAMES_IN_FLIGHT; i++)
        {
            uint64_t size = maxBuffersCount * sizeof(uint64_t);

            BufferHandle handle = MemoryRegistry::CreateGPUBuffer(size);

            BDABuffers[i] = handle;

            VkDescriptorBufferInfo bufferWriteInfo
            {
                .buffer = MemoryRegistry::GetBuffer(handle).Buffer,
                .offset = 0,
                .range = maxBuffersCount * sizeof(uint64_t)
            };

            bufferWritesInfo.push_back(bufferWriteInfo);

            VkWriteDescriptorSet writeInfo
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = sets[i],
                .dstBinding = 9,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .pBufferInfo = &bufferWritesInfo[i]
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
                .Size = maxBuffersCount * sizeof(uint64_t),
                .Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            };

            BufferHandle handle = MemoryRegistry::CreateCPUBuffer(bufferInfo);
            
            stagingBufferHandles[i] = handle;

            BufferObject& buffer = MemoryRegistry::GetBuffer(handle);

            stagingBuffers[i] = buffer;
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

void ResourceMapper::MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex)
{
    descriptorSetWrites.clear();
    buffersAddress.clear();
    copyRegions.clear();

    std::vector<VkDescriptorImageInfo> imagesSampled1DInfo;
    std::vector<VkDescriptorImageInfo> imagesStorage1DInfo;
    std::vector<VkDescriptorImageInfo> imagesSampled2DInfo;
    std::vector<VkDescriptorImageInfo> imagesStorage2DInfo;
    std::vector<VkDescriptorImageInfo> imagesSampled3DInfo;
    std::vector<VkDescriptorImageInfo> imagesStorage3DInfo;
    std::vector<VkDescriptorImageInfo> imagesSampledCubeInfo;
    std::vector<VkDescriptorImageInfo> imagesStorageCubeInfo;
    std::vector<VkDescriptorImageInfo> samplersInfo;

    imagesSampled1DInfo.reserve(imagesSampled1DToMap.size());
    imagesStorage1DInfo.reserve(imagesStorage1DToMap.size());
    imagesSampled2DInfo.reserve(imagesSampled2DToMap.size());
    imagesStorage2DInfo.reserve(imagesStorage2DToMap.size());
    imagesSampled3DInfo.reserve(imagesSampled3DToMap.size());
    imagesStorage3DInfo.reserve(imagesStorage3DToMap.size());
    imagesSampledCubeInfo.reserve(imagesSampledCubeToMap.size());
    imagesStorageCubeInfo.reserve(imagesStorageCubeToMap.size());
    samplersInfo.reserve(samplersToMap.size());

    // --- Textures 1D ---
    for(uint32_t i = 0; i < imagesSampled1DToMap.size(); i++)
    {
        TextureToMap& resource = imagesSampled1DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesSampled1DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
        };

        imagesSampled1DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 0,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imagesSampled1DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }
    for(uint32_t i = 0; i < imagesStorage1DToMap.size(); i++)
    {
        TextureToMap& resource = imagesStorage1DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesStorage1DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL
        };

        imagesStorage1DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 1,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imagesStorage1DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // --- Textures 2D ---
    for(uint32_t i = 0; i < imagesSampled2DToMap.size(); i++)
    {
        TextureToMap& resource = imagesSampled2DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesSampled2DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
        };

        imagesSampled2DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 2,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imagesSampled2DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }
    for(uint32_t i = 0; i < imagesStorage2DToMap.size(); i++)
    {
        TextureToMap& resource = imagesStorage2DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesStorage2DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL
        };

        imagesStorage2DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 3,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imagesStorage2DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // --- Textures 3D ---
    for(uint32_t i = 0; i < imagesSampled3DToMap.size(); i++)
    {
        TextureToMap& resource = imagesSampled3DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesSampled3DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
        };

        imagesSampled3DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 4,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imagesSampled3DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }
    for(uint32_t i = 0; i < imagesStorage3DToMap.size(); i++)
    {
        TextureToMap& resource = imagesStorage3DToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesStorage3DToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL
        };

        imagesStorage3DInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 5,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imagesStorage3DInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // --- Textures Cube ---
    for(uint32_t i = 0; i < imagesSampledCubeToMap.size(); i++)
    {
        TextureToMap& resource = imagesSampledCubeToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesSampledCubeToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL
        };

        imagesSampledCubeInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 6,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &imagesSampledCubeInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }
    for(uint32_t i = 0; i < imagesStorageCubeToMap.size(); i++)
    {
        TextureToMap& resource = imagesStorageCubeToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            imagesStorageCubeToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = VK_NULL_HANDLE,
            .imageView = resource.ImageView,
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL
        };

        imagesStorageCubeInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 7,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &imagesStorageCubeInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // --- Samplers ---
    for(uint32_t i = 0; i < samplersToMap.size(); i++)
    {
        SamplerToMap& resource = samplersToMap[i];

        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            samplersToMapFreeSlots.push_back(i);
        }

        VkDescriptorImageInfo imageInfo
        {
            .sampler = resource.Sampler,
            .imageView = VK_NULL_HANDLE,
            .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        samplersInfo.push_back(imageInfo);

        VkWriteDescriptorSet writeInfo
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = sets[frameIndex],
            .dstBinding = 8,
            .dstArrayElement = resource.Id,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &samplersInfo.back()
        };

        descriptorSetWrites.emplace_back(writeInfo);
    }

    // --- Buffers ---
    uint32_t srcOffset = 0;
    for (uint32_t i = 0; i < buffersToMap.size(); i++)
    {
        BufferToMap& resource = buffersToMap[i];
        if(resource.Countdown == 0) { continue; }

        resource.Countdown--;

        if(resource.Countdown == 0)
        {
            buffersToMapFreeSlots.push_back(i);
        }

        VkBufferDeviceAddressInfo addressInfo
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
            .buffer = resource.Buffer
        };

        VkDeviceAddress address = vkGetBufferDeviceAddress(GraphicsCore::Context.Device, &addressInfo);

        buffersAddress.push_back(address);

        VkBufferCopy region
        {
            .srcOffset = srcOffset * sizeof(uint64_t),
            .dstOffset = resource.Id * sizeof(uint64_t),
            .size = sizeof(uint64_t)
        };

        copyRegions.push_back(region);

        srcOffset++;
    }

    if(!descriptorSetWrites.empty())
    {
        vkUpdateDescriptorSets(GraphicsCore::Context.Device, descriptorSetWrites.size(), 
        descriptorSetWrites.data(), 0, nullptr);
    }

    if(buffersAddress.empty()) { return; }
    
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
}

void  ResourceMapper::ScheduleImageMapping(TextureHandle handle, VkImageView imageView, TextureInfo& textureInfo)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    std::vector<TextureToMap>& sampledImagesToMap = GetSampledVector(textureInfo.TextureType);
    std::vector<TextureToMap>& storageImagesToMap = GetStorageVector(textureInfo.TextureType);
    std::vector<uint32_t>& freeSampledSlots = GetFreeSampledSlotsVector(textureInfo.TextureType);
    std::vector<uint32_t>& freeStorageSlots = GetFreeStorageSlotsVector(textureInfo.TextureType);

    bool sampled = IsSampled(textureInfo.Usage);
    bool storage = IsStorage(textureInfo.Usage);

    TextureToMap resource
    {
        .ImageView = imageView,
        .Id = handle.Id,
        .Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    if(sampled && freeSampledSlots.empty())
    {
        sampledImagesToMap.push_back(resource);
    }
    else if(sampled)
    {
        uint32_t index = freeSampledSlots.back();
        freeSampledSlots.pop_back();

        sampledImagesToMap[index] = resource;
    }

    if(storage && freeStorageSlots.empty())
    {
        storageImagesToMap.push_back(resource);
    }
    else if(storage)
    {
        uint32_t index = freeStorageSlots.back();
        freeStorageSlots.pop_back();

        storageImagesToMap[index] = resource;
    }
}

void ResourceMapper::ScheduleImageMapping(TransientTextureHandle handle, VkImageView imageView, TextureInfo& textureInfo)
{
    std::lock_guard<std::mutex> lock(imagesMutex);

    std::vector<TextureToMap>& sampledImagesToMap = GetSampledVector(textureInfo.TextureType);
    std::vector<TextureToMap>& storageImagesToMap = GetStorageVector(textureInfo.TextureType);
    std::vector<uint32_t>& freeSampledSlots = GetFreeSampledSlotsVector(textureInfo.TextureType);
    std::vector<uint32_t>& freeStorageSlots = GetFreeStorageSlotsVector(textureInfo.TextureType);

    bool sampled = IsSampled(textureInfo.Usage);
    bool storage = IsStorage(textureInfo.Usage);

    TextureToMap resource
    {
        .ImageView = imageView,
        .Id = handle.Id,
        .Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    if(sampled && freeSampledSlots.empty())
    {
        sampledImagesToMap.push_back(resource);
    }
    else if(sampled)
    {
        uint32_t index = freeSampledSlots.back();
        freeSampledSlots.pop_back();

        sampledImagesToMap[index] = resource;
    }

    if(storage && freeStorageSlots.empty())
    {
        storageImagesToMap.push_back(resource);
    }
    else if(storage)
    {
        uint32_t index = freeStorageSlots.back();
        freeStorageSlots.pop_back();

        storageImagesToMap[index] = resource;
    }
}

void ResourceMapper::ScheduleSamplerMapping(SamplerHandle handle, VkSampler sampler)
{
    std::lock_guard<std::mutex> lock(samplersMutex);

    SamplerToMap resource
    {
        .Sampler = sampler,
        .Id = handle.Id,
        .Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    if(samplersToMapFreeSlots.empty())
    {
        samplersToMap.push_back(resource);
    }
    else 
    {
        uint32_t index = samplersToMapFreeSlots.back();
        samplersToMapFreeSlots.pop_back();
        
        samplersToMap[index] = resource;
    }
}

void ResourceMapper::ScheduleBufferMapping(BufferHandle handle, VkBuffer buffer)
{
    std::lock_guard<std::mutex> lock(buffersMutex);

    BufferToMap resource
    {
        .Buffer = buffer,
        .Id = handle.Id,
        .Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    if(buffersToMapFreeSlots.empty())
    {
        buffersToMap.push_back(resource);
    }
    else 
    {
        uint32_t index = buffersToMapFreeSlots.back();
        buffersToMapFreeSlots.pop_back();
        
        buffersToMap[index] = resource;
    }
}

void ResourceMapper::ScheduleBufferMapping(TransientBufferHandle handle, VkBuffer buffer)
{
    std::lock_guard<std::mutex> lock(buffersMutex);

    BufferToMap resource
    {
        .Buffer = buffer,
        .Id = handle.Id,
        .Countdown = Eve::Settings::MAX_FRAMES_IN_FLIGHT
    };

    if(buffersToMapFreeSlots.empty())
    {
        buffersToMap.push_back(resource);
    }
    else 
    {
        uint32_t index = buffersToMapFreeSlots.back();
        buffersToMapFreeSlots.pop_back();
        
        buffersToMap[index] = resource;
    }
}

std::vector<ResourceMapper::TextureToMap>& ResourceMapper::GetSampledVector(TextureType textureType)
{
    switch(textureType)
    {
        case(TextureType::TEXTURE_1D) :
            return imagesSampled1DToMap;
            break;
        case(TextureType::TEXTURE_2D) :
            return imagesSampled2DToMap;
            break;
        case(TextureType::TEXTURE_3D) :
            return imagesSampled3DToMap;
            break;
        case(TextureType::TEXTURE_CUBE) :
            return imagesSampledCubeToMap;
            break;
    }
}

std::vector<ResourceMapper::TextureToMap>& ResourceMapper::GetStorageVector(TextureType textureType)
{
    switch(textureType)
    {
        case(TextureType::TEXTURE_1D) :
            return imagesStorage1DToMap;
            break;
        case(TextureType::TEXTURE_2D) :
            return imagesStorage2DToMap;
            break;
        case(TextureType::TEXTURE_3D) :
            return imagesStorage3DToMap;
            break;
        case(TextureType::TEXTURE_CUBE) :
            return imagesStorageCubeToMap;
            break;
    }
}

std::vector<uint32_t>& ResourceMapper::GetFreeSampledSlotsVector(TextureType textureType)
{
    switch(textureType)
    {
        case(TextureType::TEXTURE_1D) :
            return imagesSampled1DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_2D) :
            return imagesSampled2DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_3D) :
            return imagesSampled3DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_CUBE) :
            return imagesSampledCubeToMapFreeSlots;
            break;
    }
}

std::vector<uint32_t>& ResourceMapper::GetFreeStorageSlotsVector(TextureType textureType)
{
    switch(textureType)
    {
        case(TextureType::TEXTURE_1D) :
            return imagesStorage1DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_2D) :
            return imagesStorage2DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_3D) :
            return imagesStorage3DToMapFreeSlots;
            break;
        case(TextureType::TEXTURE_CUBE) :
            return imagesStorageCubeToMapFreeSlots;
            break;
    }
}