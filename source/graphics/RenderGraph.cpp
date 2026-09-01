#include "RenderGraph.hpp"
#include <cstdint>
#include <graphics/registers/ShaderRegistry.hpp>
#include "builders/ContextBuilder.hpp"
#include "builders/PipelineBuilder.hpp"
#include "builders/ShaderObject.hpp"
#include "builders/Swapchain.hpp"
#include "graphics/ResourceMapper.hpp"
#include "graphics/builders/ContextBuilder.hpp"
#include "graphics/helpers/VulkanMapping.hpp"
#include "graphics/registers/MemoryRegistry.hpp"
#include "helpers/VulkanMapping.hpp"
#include "registers/MaterialRegistry.hpp"
#include "registers/ResourceTracker.hpp"
#include <graphics/registers/MeshRegistry.hpp>
#include <graphics/registers/TransientResourcePool.hpp>
#include <graphics/ResourceMapper.hpp>
#include <graphics/registers/ResourceRegistry.hpp>
#include <graphics/ErrorManager.hpp>


using namespace Eve::Graphics;

namespace 
{
    VkImageUsageFlags GetTextureUsage(Usage usage)
    {
        switch (usage)
        {
            case (Usage::VERTEX_READ_TEXTURE_SAMPLED) :
                return VK_IMAGE_USAGE_SAMPLED_BIT;

            case (Usage::FRAGMENT_READ_TEXTURE_SAMPLED) :
                return VK_IMAGE_USAGE_SAMPLED_BIT;
            
            case (Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED) :
                return VK_IMAGE_USAGE_SAMPLED_BIT;

            case (Usage::COMPUTE_READ_TEXTURE_STORAGE) :
                return VK_IMAGE_USAGE_STORAGE_BIT;

            case (Usage::COMPUTE_READ_WRITE_TEXTURE_STORAGE) :
                return VK_IMAGE_USAGE_STORAGE_BIT;

            case (Usage::COLOR_ATTACHMENT) :
                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 

            case (Usage::DEPTH_STENCIL) :
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; 

            case (Usage::DEPTH) :
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; 

            case (Usage::STENCIL) :
                return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

            case (Usage::COPY_SOURCE) :
                return VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

            case (Usage::COPY_DESTINATION) :
                return VK_IMAGE_USAGE_TRANSFER_DST_BIT;

            default : return 0;
        }
    }
    VkBufferUsageFlags GetBufferUsage(Usage usage)
    {
        switch (usage)
        {
            case(Usage::VERTEX_READ_BUFFER_STORAGE) :
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            
            case(Usage::VERTEX_READ_BUFFER_UNIFORM) :
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            
            case(Usage::FRAGMENT_READ_BUFFER_STORAGE) :
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            case(Usage::FRAGMENT_READ_BUFFER_UNIFORM) :
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            
            case(Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE) :
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            
            case(Usage::VERTEX_FRAGMENT_READ_BUFFER_UNIFORM) :
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            case(Usage::COMPUTE_READ_BUFFER_STORAGE) :
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            case(Usage::COMPUTE_READ_BUFFER_UNIFORM) :
                return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

            case(Usage::COMPUTE_READ_WRITE_BUFFER_STORAGE) :
                return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            case(Usage::COPY_SOURCE) :
                return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            case(Usage::COPY_DESTINATION) :
                return VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            default: return 0;
        }
    }

    bool IsReadOnly(Usage usage)
    {
        switch (usage) 
        {
            case(Usage::COMPUTE_READ_WRITE_TEXTURE_STORAGE) :
                return false;

            case(Usage::COMPUTE_READ_WRITE_BUFFER_STORAGE) :
                return false;

            case(Usage::COLOR_ATTACHMENT) :
                return false;
            
            case(Usage::DEPTH_STENCIL) :
                return false;

            case(Usage::DEPTH) :
                return false;

            case(Usage::STENCIL) :
                return false;

            case(Usage::COPY_DESTINATION) :
                return false;
            
            default:
                return true;
        }
    }

    bool NeedTextureDescriptor(VkImageUsageFlags usage)
    {
        if((usage & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            return true;
        }

        if((usage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
        {
            return true;
        }

        return false;
    }

    bool NeedBufferDescriptor(VkBufferUsageFlags usage)
    {
        if((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        {
            return true;
        }

        if((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        {
            return true;
        }

        return false;
    }

    RenderGraph::TextureBarrierInfo CalculateTextureBarrierInfo(Usage usage)
    {
        switch(usage)
        {
            case(Usage::VERTEX_READ_TEXTURE_SAMPLED) :
                return 
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                };

            case(Usage::FRAGMENT_READ_TEXTURE_SAMPLED) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                };

            case(Usage::VERTEX_FRAGMENT_READ_TEXTURE_SAMPLED) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                };

            case(Usage::COMPUTE_READ_TEXTURE_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_GENERAL
                };

            case(Usage::COMPUTE_READ_WRITE_TEXTURE_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT_KHR | VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_GENERAL
                };
            case(Usage::COLOR_ATTACHMENT) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::DEPTH_STENCIL) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::DEPTH) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::STENCIL) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT_KHR | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT_KHR | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::COPY_SOURCE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_TRANSFER_READ_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
                };

            case(Usage::COPY_DESTINATION) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR,
                    .Layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                };

            default:
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
                    .AccessMask = VK_ACCESS_2_NONE_KHR,
                    .Layout = VK_IMAGE_LAYOUT_UNDEFINED
                };
        }
    }
    RenderGraph::BufferBarrierInfo CalculateBufferBarrierInfo(Usage usage)
    {
        switch(usage)
        {
            case(Usage::VERTEX_READ_BUFFER_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR
                };
            
            case(Usage::VERTEX_READ_BUFFER_UNIFORM) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_UNIFORM_READ_BIT_KHR
                };

            case(Usage::FRAGMENT_READ_BUFFER_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR
                };
            
            case(Usage::FRAGMENT_READ_BUFFER_UNIFORM) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_UNIFORM_READ_BIT_KHR
                };

            case(Usage::VERTEX_FRAGMENT_READ_BUFFER_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR
                };
            
            case(Usage::VERTEX_FRAGMENT_READ_BUFFER_UNIFORM) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_UNIFORM_READ_BIT_KHR
                };
            
            case(Usage::COMPUTE_READ_BUFFER_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR
                };

            case(Usage::COMPUTE_READ_BUFFER_UNIFORM) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_UNIFORM_READ_BIT_KHR
                };
            
            case(Usage::COMPUTE_READ_WRITE_BUFFER_STORAGE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT_KHR | VK_ACCESS_2_SHADER_STORAGE_READ_BIT_KHR
                };

            case(Usage::COPY_SOURCE) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_TRANSFER_READ_BIT_KHR,
                };

            case(Usage::COPY_DESTINATION) :
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT_KHR,
                    .AccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT_KHR,
                };

            default:
                return
                {
                    .StageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
                    .AccessMask = VK_ACCESS_2_NONE_KHR,
                };
            
        }
    }

    void CreateTransientTexture(TextureInfo& textureInfo, VkImage& image)
    {
        VkImageType imageType;
        VkImageCreateFlags flags = static_cast<VkImageCreateFlags>(0);
        
        switch (textureInfo.TextureType) 
        {
            case (TextureType::TEXTURE_1D):
                imageType = VK_IMAGE_TYPE_1D;
                break;
            case (TextureType::TEXTURE_2D):
                imageType = VK_IMAGE_TYPE_2D;
                break;
            case (TextureType::TEXTURE_3D):
                imageType = VK_IMAGE_TYPE_3D;
                break;
            case (TextureType::TEXTURE_CUBE):
                imageType = VK_IMAGE_TYPE_2D;
                flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
        }

        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = flags,
            .imageType = imageType,
            .format = format,
            .extent {.width = textureInfo.Width, .height = textureInfo.Height, .depth = textureInfo.Depth},
            .mipLevels = textureInfo.MipLevels,
            .arrayLayers = textureInfo.ArrayLayers,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = textureInfo.Usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vkCreateImage(GraphicsCore::Context.Device, &imageCI, nullptr, &image));
    }

    void CreateImageView(TextureInfo& textureInfo, VkImage image, VkImageView& imageView)
    {
        VkImageViewType imageViewType;
        VkImageCreateFlags flags = static_cast<VkImageCreateFlags>(0);

        switch (textureInfo.TextureType) 
        {
            case (TextureType::TEXTURE_1D):
                imageViewType = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case (TextureType::TEXTURE_2D):
                imageViewType = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case (TextureType::TEXTURE_3D):
                imageViewType = VK_IMAGE_VIEW_TYPE_3D;
                break;
            case (TextureType::TEXTURE_CUBE):
                imageViewType = VK_IMAGE_VIEW_TYPE_CUBE;
                flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
        }

        VkFormat format = GetVkImageFormat(textureInfo.Format);
        VkImageViewCreateInfo imageViewCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
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

        VK_CHECK(vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &imageView));

    }

    void CreateTransientBuffer(BufferInfo& bufferInfo, VkBuffer& buffer)
    {
        VkBufferCreateInfo bufferCI
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = bufferInfo.Size,
            .usage = bufferInfo.Usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VmaAllocationCreateInfo bufferAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        VK_CHECK(vkCreateBuffer(GraphicsCore::Context.Device, &bufferCI, nullptr, &buffer));
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

    bool IsTextureBarrierNeeded(TextureState src, RenderGraph::TextureBarrierInfo dst, Usage newUsage)
    {
        if(IsReadOnly(src.Usage) && IsReadOnly(newUsage))
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

    bool IsBufferBarrierNeeded(BufferState src, RenderGraph::BufferBarrierInfo dst, Usage newUsage)
    {
        if(IsReadOnly(src.Usage) && IsReadOnly(newUsage))
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
        bool isDedicatedGPU = GraphicsCore::Context.PhysicalDeviceInfo.isDedicated;
        int32_t score = INT32_MIN;
        int32_t memoryTypeIndex = 0;
        for(uint32_t i = 0; i < GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypeCount; i++)
        {
            bool isCompatibleBit = (memoryRequirements.memoryRequirements.memoryTypeBits & (1 << i)) != 0;

            if(!isCompatibleBit) { continue; }

            int32_t currentScore = 0;

            // Local Device?
            if(GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                currentScore += 500;
            }

            // Host Visible?
            if(isDedicatedGPU && (GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT))
            {
                currentScore -= 200;
            }

            // Host Coherent?
            if(isDedicatedGPU && (GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
            {
                currentScore -= 150;
            }

            // Lazily Allocated?
            if((GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT))
            {
                currentScore -= 1000;
            }

            // Some kink of memory which is not DEVICE_LOCAL, HOST_VISIBLE and HOST_COHERENT
            if((GraphicsCore::Context.PhysicalDeviceInfo.MemoryProperties.memoryTypes[i].propertyFlags &
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

    VkAttachmentLoadOp GetVkLoadOp(LoadOperation loadOp)
    {
        switch(loadOp)
        {
            case (LoadOperation::LOAD) :
                return VK_ATTACHMENT_LOAD_OP_LOAD;

            case (LoadOperation::DISCARD) :
                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            
            case(LoadOperation::CLEAR) : 
                return VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
    }

    VkAttachmentStoreOp GetVkStoreOp(StoreOperation storeOp)
    {
        switch(storeOp)
        {
            case (StoreOperation::STORE) :
                return VK_ATTACHMENT_STORE_OP_STORE;

            case (StoreOperation::DISCARD) :
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }
};

bool RenderGraph::Execute(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swapchainImageIndex)
{
    bool success = CompileGraph(frameIndex);

    VkCommandBufferBeginInfo beginInfo
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    VK_CHECK(vkBeginCommandBuffer(cmdBuffer, &beginInfo));

    ResourceMapper::MapResources(cmdBuffer, frameIndex);

    VkDescriptorSet descriptorSet = ResourceMapper::GetDescriptorSet(frameIndex);

    vkCmdBindDescriptorSets(
        cmdBuffer, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        PipelineBuilder::GetGraphicsPipelineLayout(), 
        0, 
        1, 
        &descriptorSet, 
        0, 
        nullptr
    );

    RecordCommands(cmdBuffer, frameIndex, swapchainImageIndex);

    VK_CHECK(vkEndCommandBuffer(cmdBuffer));

    Clear();

    return true;
}

bool RenderGraph::CompileGraph(uint32_t frameIndex)
{
    MeshRegistry::UploadMeshes();
    MaterialRegistry::UploadMaterials();

    TransientResourcePool::UpdateTexturesPool(frameIndex);
    TransientResourcePool::UpdateBuffersPool(frameIndex);

    uint32_t transientTexturesCount = transientRequestedTextures.size();
    uint32_t transientBuffersCount = transientRequestedBuffers.size();
    uint32_t passesCount = passes.size();

    for(uint32_t bucketIndex = 0; bucketIndex < texturesBucketPasses.size(); bucketIndex++)
    {
        texturesBucketPasses[bucketIndex].resize(passesCount);
    }
    for(uint32_t bucketIndex = 0; bucketIndex < buffersBucketPasses.size(); bucketIndex++)
    {
        buffersBucketPasses[bucketIndex].resize(passesCount);
    }

    // Calculate first and last passes
    // Calculate resources usage
    // Calculate resource memory info
    uint32_t texturesBarriersOffset = 0;
    int32_t lastValidTextureIndex = -1;
    barriersOffsetPerTexture.push_back(texturesBarriersOffset);
    for(uint32_t textureId = 0; textureId < transientTexturesCount; textureId++)
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

        uint32_t resourceId = transientRequestedTextureHandles[textureId].Id;

        Usage oldUsage = static_cast<Usage>(0);
        VkImageUsageFlags usage = 0;
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientTextureHandle, Usage>>& textures = passes[passIndex].transientTextures;
            for(uint32_t i = 0; i < textures.size(); i++)
            {
                // For each textures in that pass grab the texture usage
                // and calculate the first and last passes
                if(textures[i].first.Id != resourceId) { continue; }

                if(firstPassIndex == -1)
                {
                    firstPassIndex = passIndex;

                    texturesBarriersInfo.push_back(std::pair{srcBarrierInfo, 0});
                    barriersCount++;
                }

                lastPassIndex = passIndex;

                usage |= GetTextureUsage(textures[i].second);

                TextureBarrierInfo dstBarrierInfo = CalculateTextureBarrierInfo(textures[i].second);

                bool barrier = IsTextureBarrierNeeded(srcBarrierInfo, dstBarrierInfo, oldUsage, textures[i].second);

                if(barrier)
                {
                    dstBarrierInfo.TextureId = textureId;
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

        bool isUsed = !(firstPassIndex == -1);

        if(!isUsed) { continue; }

        bool isPresentTexture = resourceId == presentTexture.Id;
        if(isPresentTexture)
        {
            usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            isPresentTextureValid = true;
        }

        TextureInfo newTextureInfo = transientRequestedTextures[textureId];
        newTextureInfo.Usage = usage;
       
        TransientResourcePool::AddTextureResource(newTextureInfo, resourceId, frameIndex, textureId);

        uint32_t poolIndex = TransientResourcePool::FindTexturePoolIndex(newTextureInfo, passesCount);

        TextureResource& resource = TransientResourcePool::GetTextureObject(textureId, frameIndex);
        resource.TexturePoolIndex = poolIndex;

        TexturePool& pool = TransientResourcePool::GetTexturePool(poolIndex);
        
        texturesBucketPasses[pool.MemoryInfo.BucketIndex][firstPassIndex].TexturesToCreate.push_back(textureId);
        texturesBucketPasses[pool.MemoryInfo.BucketIndex][lastPassIndex].TexturesToDestroy.push_back(textureId);

        lastValidTextureIndex = textureId;
    }
    uint32_t buffersBarriersOffset = 0;
    int32_t lastValidBufferIndex = -1;
    barriersOffsetPerBuffer.push_back(buffersBarriersOffset);
    for(uint32_t bufferId = 0; bufferId < transientBuffersCount; bufferId++)
    {
        uint32_t barriersCount = 0;
        int32_t firstPassIndex = -1;
        int32_t lastPassIndex = -1;

        BufferBarrierInfo srcBarrierInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE
        };

        uint32_t resourceId = transientRequestedBufferHandles[bufferId].Id;

        Usage oldUsage = static_cast<Usage>(0);
        VkBufferUsageFlags usage = 0;
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientBufferHandle, Usage>>& buffers = passes[passIndex].transientBuffers;
            for(uint32_t i = 0; i < buffers.size(); i++)
            {
                // For each buffer in that pass grab the texture usage
                // and calculate the first and last passes
                if(buffers[i].first.Id != resourceId) { continue; }

                if(firstPassIndex == -1)
                {
                    firstPassIndex = passIndex;
                    buffersBarriersInfo.push_back(std::pair{srcBarrierInfo, 0});
                    barriersCount++;
                }

                lastPassIndex = passIndex;

                usage |= GetBufferUsage(buffers[i].second);

                BufferBarrierInfo dstBarrierInfo = CalculateBufferBarrierInfo(buffers[i].second);

                bool firstUse = firstPassIndex == passIndex;
                bool barrier = IsBufferBarrierNeeded(srcBarrierInfo, dstBarrierInfo, oldUsage, buffers[i].second);

                if(barrier)
                {
                    dstBarrierInfo.BufferId = bufferId;
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

        BufferInfo newBufferInfo = transientRequestedBuffers[bufferId];
        newBufferInfo.Usage = usage;
        
        TransientResourcePool::AddBufferResource(newBufferInfo, resourceId, frameIndex, bufferId);

        uint32_t poolIndex = TransientResourcePool::FindBufferPoolIndex(newBufferInfo, passesCount);

        BufferResource& resource = TransientResourcePool::GetBufferObject(bufferId, frameIndex);
        resource.BufferPoolIndex = poolIndex;

        BufferPool& pool = TransientResourcePool::GetBufferPool(poolIndex);

        buffersBucketPasses[pool.MemoryInfo.BucketIndex][firstPassIndex].BuffersToCreate.push_back(bufferId);
        buffersBucketPasses[pool.MemoryInfo.BucketIndex][lastPassIndex].BuffersToDestroy.push_back(bufferId);

        lastValidBufferIndex = bufferId;        
    }

    transientRequestedTextureHandles.clear();
    transientRequestedBufferHandles.clear();

    // Generate persistent texture and buffer barriers
    for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
    {
        Pass& pass = passes[passIndex];
        std::vector<std::pair<TextureHandle, Usage>>& textures = pass.persistentTextures;
        std::vector<std::pair<BufferHandle, Usage>>& buffers = pass.persistentBuffers;

        // --- Textures ---
        for(uint32_t i = 0; i < textures.size(); i++)
        {
            TextureHandle handle = textures[i].first;
            Usage newUsage = textures[i].second;

            TextureState& textureState = ResourceTracker::GetTextureState(handle);
            TextureBarrierInfo dstBarrierInfo = CalculateTextureBarrierInfo(newUsage);

            bool barrier = IsTextureBarrierNeeded(textureState, dstBarrierInfo, newUsage);

            if(barrier)
            {
                dstBarrierInfo.TextureId = handle.Id;

                TextureBarrierInfo srcBarrierInfo
                {
                    .TextureId = handle.Id,
                    .StageMask = textureState.StageMask,
                    .AccessMask = textureState.AccessMask,
                    .Layout = textureState.Layout
                };

                TextureBarrierInfoPair barrierInfo
                {
                    .SrcInfo = srcBarrierInfo,
                    .DstInfo = dstBarrierInfo
                };

                pass.persistentTexturesBarriers.push_back(barrierInfo);

                textureState.StageMask = dstBarrierInfo.StageMask;
                textureState.AccessMask = dstBarrierInfo.AccessMask;
                textureState.Layout = dstBarrierInfo.Layout;
            }
            else 
            {
                textureState.StageMask |= dstBarrierInfo.StageMask;
                textureState.AccessMask |= dstBarrierInfo.AccessMask;
            }

            textureState.Usage = newUsage;

        }

        // --- Buffers ----
        for(uint32_t i = 0; i < buffers.size(); i++)
        {
            BufferHandle handle = buffers[i].first;
            Usage newUsage = buffers[i].second;

            BufferState& bufferState = ResourceTracker::GetBufferState(handle);
            BufferBarrierInfo dstBarrierInfo = CalculateBufferBarrierInfo(newUsage);

            bool barrier = IsBufferBarrierNeeded(bufferState, dstBarrierInfo, newUsage);

            if(barrier)
            {
                dstBarrierInfo.BufferId = handle.Id;

                BufferBarrierInfo srcBarrierInfo
                {
                    .BufferId = handle.Id,
                    .StageMask = bufferState.StageMask,
                    .AccessMask = bufferState.AccessMask
                };

                BufferBarrierInfoPair barrierInfo
                {
                    .SrcInfo = srcBarrierInfo,
                    .DstInfo = dstBarrierInfo
                };

                pass.persistentBuffersBarriers.push_back(barrierInfo);

                bufferState.StageMask = dstBarrierInfo.StageMask;
                bufferState.AccessMask = dstBarrierInfo.AccessMask;
            }
            else 
            {
                bufferState.StageMask |= dstBarrierInfo.StageMask;
                bufferState.AccessMask |= dstBarrierInfo.AccessMask;
            }

            bufferState.Usage = newUsage;

        }
    }

    VmaVirtualBlockCreateInfo virtualBlockCI
    {
        .size = UINT64_MAX
    };

    VmaVirtualBlock block;
    VK_CHECK(vmaCreateVirtualBlock(&virtualBlockCI, &block));

    texturesVirtualAllocs.resize(lastValidTextureIndex + 1);
    buffersVirtualAllocs.resize(lastValidBufferIndex + 1);

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
            std::vector<uint32_t>& texturesToCreate = pass.TexturesToCreate;

            for(uint32_t i = 0; i < texturesToCreate.size(); i++)
            {
                uint32_t resourceIndex = texturesToCreate[i];

                TextureResource& resource = TransientResourcePool::GetTextureObject(resourceIndex, frameIndex);

                TexturePool& pool = TransientResourcePool::GetTexturePool(resource.TexturePoolIndex);

                VmaVirtualAllocationCreateInfo allocCI
                {
                    .size = pool.MemoryInfo.Size,
                    .alignment = pool.MemoryInfo.Alignment
                };

                VmaVirtualAllocation allocation;
                VkDeviceSize offset;
                VK_CHECK(vmaVirtualAllocate(block, &allocCI, &allocation, &offset));

                peakSize = std::max(peakSize, static_cast<uint64_t>(offset) + pool.MemoryInfo.Size);
                peakAlignment = std::max(peakAlignment, pool.MemoryInfo.Alignment);

                TextureBarrierInfo firstBarrier = GetFirstTextureBarrierInfo(resourceIndex, offset, pool.MemoryInfo.Size);
                uint32_t firstBarrierIndex = barriersOffsetPerTexture[resourceIndex];
                texturesBarriersInfo[firstBarrierIndex].first = firstBarrier;

                texturesVirtualAllocs[resourceIndex] = allocation;

                resource.MemoryOffset = static_cast<uint64_t>(offset);
            }

            // Texture Destruction
            std::vector<uint32_t>& texturesToDestroy = pass.TexturesToDestroy;

            for(uint32_t i = 0; i < texturesToDestroy.size(); i++)
            {
                uint32_t resourceIndex = texturesToDestroy[i];

                VmaVirtualAllocation allocation = texturesVirtualAllocs[resourceIndex];

                vmaVirtualFree(block, allocation);
            }

        }

        vmaClearVirtualBlock(block);
        virtualMemorySlots.clear();

        TransientResourcePool::ResizeTextureMemoryBucketIfNeeded(bucketIndex, peakSize, peakAlignment);
    }

    // Buffers Memory Aliasing
    for(uint32_t bucketIndex = 0; bucketIndex < buffersBucketPasses.size(); bucketIndex++)
    {
        uint64_t peakSize = 0;
        uint64_t peakAlignment = 0;
        std::vector<BuffersBucketPass>& _passes = buffersBucketPasses[bucketIndex];

        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            BuffersBucketPass& pass = _passes[passIndex];

            // Buffer Creation
            std::vector<uint32_t>& buffersToCreate = pass.BuffersToCreate;

            for(uint32_t i = 0; i < buffersToCreate.size(); i++)
            {
                uint32_t resourceIndex = buffersToCreate[i];

                BufferResource& resource = TransientResourcePool::GetBufferObject(resourceIndex, frameIndex);

                BufferPool& pool = TransientResourcePool::GetBufferPool(resource.BufferPoolIndex);

                VmaVirtualAllocationCreateInfo allocCI
                {
                    .size = pool.MemoryInfo.Size,
                    .alignment = pool.MemoryInfo.Alignment
                };

                VmaVirtualAllocation allocation;
                VkDeviceSize offset;
                VK_CHECK(vmaVirtualAllocate(block, &allocCI, &allocation, &offset));

                peakSize = std::max(peakSize, static_cast<uint64_t>(offset) + pool.MemoryInfo.Size);
                peakAlignment = std::max(peakAlignment, pool.MemoryInfo.Alignment);

                BufferBarrierInfo firstBarrier = GetFirstBufferBarrierInfo(resourceIndex, offset, pool.MemoryInfo.Size);
                uint32_t firstBarrierIndex = barriersOffsetPerBuffer[resourceIndex];
                buffersBarriersInfo[firstBarrierIndex].first = firstBarrier;

                buffersVirtualAllocs[resourceIndex] = allocation;

                resource.MemoryOffset = static_cast<uint64_t>(offset);
            }

            // Buffer Destruction
            std::vector<uint32_t>& buffersToDestroy = pass.BuffersToDestroy;

            for(uint32_t i = 0; i < buffersToDestroy.size(); i++)
            {
                uint32_t resourceIndex = buffersToDestroy[i];

                VmaVirtualAllocation allocation = buffersVirtualAllocs[resourceIndex];

                vmaVirtualFree(block, allocation);
            }
        }

        vmaClearVirtualBlock(block);
        virtualMemorySlots.clear();

        TransientResourcePool::ResizeBufferMemoryBucketIfNeeded(bucketIndex, peakSize, peakAlignment);
    }

    texturesVirtualAllocs.clear();
    buffersVirtualAllocs.clear();

    vmaDestroyVirtualBlock(block);

    // Create/Reuse textures and buffers
    // Insert the barriers info in the right passes so the barriers can be created later
    for(uint32_t textureId = 0; textureId < static_cast<uint32_t>(lastValidTextureIndex + 1); textureId++)
    {
        TextureResource& resource = TransientResourcePool::GetTextureObject(textureId, frameIndex);

        if(!resource.isValid) { continue; }

        uint64_t memoryOffset = resource.MemoryOffset;

        TexturePool& pool = TransientResourcePool::GetTexturePool(resource.TexturePoolIndex);

        std::vector<TransientTextureObject>& pooledResources = pool.Textures;

        bool found = false;
        uint32_t foundResourceIndex;
        for(uint32_t i = 0; i < pooledResources.size(); i++)
        {
            TransientTextureObject& foundResource = pooledResources[i];

            if(foundResource.PooledResource == false && foundResource.MemoryOffset == memoryOffset)
            {
                found = true;
                foundResource.PooledResource = true;
                foundResourceIndex = i;
            }

        }

        if(found)
        {
            TransientTextureObject& foundResource = pooledResources[foundResourceIndex];

            resource.Image = foundResource.Image;
            resource.ImageView = foundResource.ImageView;
            resource.PooledResource = true;

            TransientTextureHandle handle = {resource.Id};

            bool shouldBeMapped = NeedTextureDescriptor(pool.TextureInfo.Usage);

            if(shouldBeMapped)
            {
                ResourceMapper::ScheduleImageMapping(resource.Id, resource.ImageView, pool.TextureInfo);
            };
        }
        else 
        {
            VkImage image;
            CreateTransientTexture(pool.TextureInfo, image);

            resource.Image = image;
            resource.PooledResource = false;

            MemoryBucket& memoryPool = TransientResourcePool::GetTextureMemoryBucket(pool.MemoryInfo.BucketIndex);
            VK_CHECK(vmaBindImageMemory2(GraphicsCore::Context.Allocator, memoryPool.Allocation, 
                memoryOffset, image, nullptr));

            VkImageView imageView;
            CreateImageView(pool.TextureInfo, image, imageView);

            resource.ImageView = imageView;

            TransientTextureHandle handle = {resource.Id};

            bool shouldBeMapped = NeedTextureDescriptor(pool.TextureInfo.Usage);

            if(shouldBeMapped)
            {
                ResourceMapper::ScheduleImageMapping(handle, imageView, pool.TextureInfo);
            }
        }

        // Insert barriers in their sync points
        uint32_t offset = barriersOffsetPerTexture[textureId];
        uint32_t barriersCount = barriersOffsetPerTexture[textureId + 1] - offset;
        for(uint32_t i = 1; i < barriersCount; i++)
        {
            uint32_t index = i + offset;

            TextureBarrierInfo& srcBarrierInfo = texturesBarriersInfo[index - 1].first;
            TextureBarrierInfo& dstBarrierInfo = texturesBarriersInfo[index].first;
            uint32_t passIndex = texturesBarriersInfo[index].second;

            passes[passIndex].transientTexturesBarriers.emplace_back(srcBarrierInfo, dstBarrierInfo);
        }
    }
    for(uint32_t bufferId = 0; bufferId < static_cast<uint32_t>(lastValidBufferIndex + 1); bufferId++)
    {
        BufferResource& resource = TransientResourcePool::GetBufferObject(bufferId, frameIndex);

        if(!resource.isValid) { continue; }

        uint64_t memoryOffset = resource.MemoryOffset;

        BufferPool& pool = TransientResourcePool::GetBufferPool(resource.BufferPoolIndex);

        std::vector<TransientBufferObject>& pooledResources = pool.Buffers;

        bool found = false;
        uint32_t foundResourceIndex;
        for(uint32_t i = 0; i < pooledResources.size(); i++)
        {
            TransientBufferObject& foundResource = pooledResources[i];

            if(foundResource.PooledResource == false && foundResource.MemoryOffset == memoryOffset)
            {
                found = true;
                foundResource.PooledResource = true;
                foundResourceIndex = i;
            }

        }

        if(found)
        {
            TransientBufferObject& foundResource = pooledResources[foundResourceIndex];

            resource.Buffer = foundResource.Buffer;
            resource.PooledResource = true;

            TransientBufferHandle handle = { resource.Id };

            bool shouldBeMapped = NeedBufferDescriptor(pool.BufferInfo.Usage);

            if(shouldBeMapped)
            {
                ResourceMapper::ScheduleBufferMapping(handle, resource.Buffer);
            }
        }
        else 
        {
            VkBuffer buffer;
            CreateTransientBuffer(pool.BufferInfo, buffer);

            resource.Buffer = buffer;
            resource.PooledResource = false;

            MemoryBucket& memoryPool = TransientResourcePool::GetBufferMemoryBucket(pool.MemoryInfo.BucketIndex);
            VK_CHECK(vmaBindBufferMemory2(GraphicsCore::Context.Allocator, memoryPool.Allocation, 
                memoryOffset, buffer, nullptr));

            TransientBufferHandle handle = { resource.Id };

            bool shouldBeMapped = NeedBufferDescriptor(pool.BufferInfo.Usage);

            if(shouldBeMapped)
            {
                ResourceMapper::ScheduleBufferMapping(handle, buffer);
            }
        }

        // Insert barriers in their sync points
        uint32_t offset = barriersOffsetPerBuffer[bufferId];
        uint32_t barriersCount = barriersOffsetPerBuffer[bufferId + 1] - offset;
        for(uint32_t i = 1; i < barriersCount; i++)
        {
            uint32_t index = i + offset;

            BufferBarrierInfo& srcBarrierInfo = buffersBarriersInfo[index - 1].first;
            BufferBarrierInfo& dstBarrierInfo = buffersBarriersInfo[index].first;
            uint32_t passIndex = buffersBarriersInfo[index].second;

            passes[passIndex].transientBuffersBarriers.emplace_back(srcBarrierInfo, dstBarrierInfo);
        }
    }

    return true;
}

bool RenderGraph::RecordCommands(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swapchainImageIndex)
{
    std::vector<VkImageMemoryBarrier2KHR> textureMemoryBarriers;
    std::vector<VkBufferMemoryBarrier2KHR> bufferMemoryBarriers;

    uint32_t passCount = passes.size();
    for(uint32_t passIndex = 0; passIndex < passCount; passIndex++)
    {
        Pass& pass = passes[passIndex];
        
        std::vector<TextureBarrierInfoPair>& transientTextureBarriers = pass.transientTexturesBarriers;
        std::vector<BufferBarrierInfoPair>& transientBufferBarriers = pass.transientBuffersBarriers;
        std::vector<TextureBarrierInfoPair>& persistentTextureBarriers = pass.persistentTexturesBarriers;
        std::vector<BufferBarrierInfoPair>& persistentBufferBarriers = pass.persistentBuffersBarriers;

        // --- Record Transient Textures Barriers ---
        for(uint32_t i = 0; i < transientTextureBarriers.size(); i++)
        {
            TextureBarrierInfoPair barrierInfo = transientTextureBarriers[i];

            uint32_t textureIndex = barrierInfo.DstInfo.TextureId;

            TextureResource& texture = TransientResourcePool::GetTextureObject(textureIndex, frameIndex);
            VkImage image = texture.Image;
            VkFormat format = GetVkImageFormat(texture.TextureInfo.Format);
            VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

            VkImageMemoryBarrier2KHR barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .oldLayout = barrierInfo.SrcInfo.Layout,
                .newLayout = barrierInfo.DstInfo.Layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange
                {
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = texture.TextureInfo.ArrayLayers
                }
            };

            textureMemoryBarriers.push_back(barrier);
        }

        // --- Record Transient Buffers Barriers ---
        for(uint32_t i = 0; i < transientBufferBarriers.size(); i++)
        {
            BufferBarrierInfoPair barrierInfo = transientBufferBarriers[i];

            uint32_t bufferIndex = barrierInfo.DstInfo.BufferId;

            VkBuffer buffer = TransientResourcePool::GetBufferObject(bufferIndex, frameIndex).Buffer;

            VkBufferMemoryBarrier2KHR barrier
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buffer,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            };

            bufferMemoryBarriers.push_back(barrier);
        }

        // --- Record Persistent Textures Barriers ---
        for(uint32_t i = 0; i < persistentTextureBarriers.size(); i++)
        {
            TextureBarrierInfoPair barrierInfo = persistentTextureBarriers[i];

            uint32_t textureId = barrierInfo.DstInfo.TextureId;

            VkImage image = MemoryRegistry::GetTexture(textureId).Image;

            TextureInfo& textureInfo = MemoryRegistry::GetTextureInfo(textureId);

            VkFormat format = GetVkImageFormat(textureInfo.Format);
            VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

            VkImageMemoryBarrier2KHR barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .oldLayout = barrierInfo.SrcInfo.Layout,
                .newLayout = barrierInfo.DstInfo.Layout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = image,
                .subresourceRange
                {
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = textureInfo.ArrayLayers
                }
            };

            textureMemoryBarriers.push_back(barrier);
        }

        // --- Record Persistent Buffers Barriers ---
        for(uint32_t i = 0; i < persistentBufferBarriers.size(); i++)
        {
            BufferBarrierInfoPair barrierInfo = persistentBufferBarriers[i];

            uint32_t bufferId = barrierInfo.DstInfo.BufferId;

            VkBuffer buffer = MemoryRegistry::GetBuffer(bufferId).Buffer;

            VkBufferMemoryBarrier2KHR barrier
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2_KHR,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = buffer,
                .offset = 0,
                .size = VK_WHOLE_SIZE
            };

            bufferMemoryBarriers.push_back(barrier);
        }

        VkDependencyInfoKHR dependency
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
            .bufferMemoryBarrierCount = static_cast<uint32_t>(bufferMemoryBarriers.size()),
            .pBufferMemoryBarriers = bufferMemoryBarriers.data(),
            .imageMemoryBarrierCount = static_cast<uint32_t>(textureMemoryBarriers.size()),
            .pImageMemoryBarriers = textureMemoryBarriers.data()
        };

        if(bufferMemoryBarriers.size() > 0 || textureMemoryBarriers.size() > 0)
        {
            vkCmdPipelineBarrier2KHR(cmdBuffer, &dependency);
        }

        textureMemoryBarriers.clear();
        bufferMemoryBarriers.clear();

        // --- Transient resource copies ---
        RecordTransientBufferCopy(cmdBuffer, pass, frameIndex);
        RecordTransientTextureCopy(cmdBuffer, pass, frameIndex);
        RecordTransientBufferToTextureCopy(cmdBuffer, pass, frameIndex);
        RecordTransientTextureToBufferCopy(cmdBuffer, pass, frameIndex);

        // --- Transient resouce uploads ---
        RecordTransientBufferUpload(cmdBuffer, pass, frameIndex);
        RecordTransientTextureUpload(cmdBuffer, pass, frameIndex);

        // --- Persistent resource copies ---
        RecordPersistentBufferCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTextureCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentBufferToTextureCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTextureToBufferCopy(cmdBuffer, pass, frameIndex);

        // --- Persistent resource uploads ---
        RecordPersistentBufferUpload(cmdBuffer, pass, frameIndex);
        RecordPersistentTextureUpload(cmdBuffer, pass, frameIndex);

        // --- Buffer copies mix ---
        RecordTransientPersistentBufferCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTransientBufferCopy(cmdBuffer, pass, frameIndex);

        // --- Texture copies mix ---
        RecordTransientPersistentTextureCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTransientTextureCopy(cmdBuffer, pass, frameIndex);

        // --- Buffer To Texture copies mix ---
        RecordTransientPersistentBufferToTextureCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTransientBufferToTextureCopy(cmdBuffer, pass, frameIndex);

        // --- Texture To Buffer copies mix ---
        RecordTransientPersistentTextureToBufferCopy(cmdBuffer, pass, frameIndex);
        RecordPersistentTransientTextureToBufferCopy(cmdBuffer, pass, frameIndex);

        RecordDrawCalls(cmdBuffer, pass, frameIndex);
    }

    RecordSwapchainDrawingPass(cmdBuffer, frameIndex, swapchainImageIndex);

    return true;
}

void RenderGraph::Clear()
{
    transientRequestedTextures.clear();
    transientRequestedBuffers.clear();
    passes.clear();
    transientTextureHandleToIndex.clear();
    transientBufferHandleToIndex.clear();

    for(uint32_t i = 0; i < texturesBucketPasses.size(); i++)
    {
        texturesBucketPasses[i].clear();
    }
    for(uint32_t i = 0; i < buffersBucketPasses.size(); i++)
    {
        buffersBucketPasses[i].clear();
    }
    
    barriersOffsetPerTexture.clear();
    texturesBarriersInfo.clear();
    barriersOffsetPerBuffer.clear();
    buffersBarriersInfo.clear();

    presentTexture.Id = UINT32_MAX;
    isPresentTextureValid = false;
}

void RenderGraph::RecordTransientBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferCopy>& copies = pass.transientBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferCopy copyInfo = copies[i];

        VkBuffer srcBuffer = TransientResourcePool::GetBufferObject(copyInfo.SrcBuffer, frameIndex).Buffer;
        VkBuffer dstBuffer = TransientResourcePool::GetBufferObject(copyInfo.DstBuffer, frameIndex).Buffer;

        VkBufferCopy bufferCopy
        {
            .srcOffset = copyInfo.SrcOffset,
            .dstOffset = copyInfo.DstOffset,
            .size = copyInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    }
}

void RenderGraph::RecordPersistentBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferCopy>& copies = pass.persistentBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferCopy copyInfo = copies[i];

        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(copyInfo.SrcBuffer).Buffer;
        VkBuffer dstBuffer = MemoryRegistry::GetBuffer(copyInfo.DstBuffer).Buffer;

        VkBufferCopy bufferCopy
        {
            .srcOffset = copyInfo.SrcOffset,
            .dstOffset = copyInfo.DstOffset,
            .size = copyInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    }
}

void RenderGraph::RecordTransientTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureCopy>& copies = pass.transientTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureCopy copyInfo = copies[i];

        TextureResource& srcTexture = TransientResourcePool::GetTextureObject(copyInfo.SrcTexture, frameIndex);
        TextureResource& dstTexture = TransientResourcePool::GetTextureObject(copyInfo.DstTexture, frameIndex);

        VkImage srcImage = srcTexture.Image;
        VkImage dstImage = dstTexture.Image;

        VkFormat srcFormat = GetVkImageFormat(srcTexture.TextureInfo.Format);
        VkFormat dstFormat = GetVkImageFormat(dstTexture.TextureInfo.Format);

        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkImageCopy imageCopy
        {
            .srcSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.TextureInfo.ArrayLayers
            },
            .srcOffset {copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},

            .dstSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.TextureInfo.ArrayLayers
            },
            .dstOffset {copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},

            .extent {
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y),
                static_cast<uint32_t>(copyInfo.Extent.z)
            }
        };

        vkCmdCopyImage(cmdBuffer, 
            srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }
}

void RenderGraph::RecordPersistentTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureCopy>& copies = pass.persistentTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureCopy copyInfo = copies[i];

        TextureInfo& srcTexture = MemoryRegistry::GetTextureInfo(copyInfo.SrcTexture);
        TextureInfo& dstTexture = MemoryRegistry::GetTextureInfo(copyInfo.DstTexture);

        VkImage srcImage = MemoryRegistry::GetTexture(copyInfo.SrcTexture).Image;
        VkImage dstImage = MemoryRegistry::GetTexture(copyInfo.DstTexture).Image;

        VkFormat srcFormat = GetVkImageFormat(srcTexture.Format);
        VkFormat dstFormat = GetVkImageFormat(dstTexture.Format);

        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkImageCopy imageCopy
        {
            .srcSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.ArrayLayers
            },
            .srcOffset {copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},

            .dstSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.ArrayLayers
            },
            .dstOffset {copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},

            .extent {
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y),
                static_cast<uint32_t>(copyInfo.Extent.z)
            }
        };

        vkCmdCopyImage(cmdBuffer, 
            srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }
}

void RenderGraph::RecordTransientBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferToTextureCopy>& copies = pass.transientBufferToTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferToTextureCopy copyInfo = copies[i];

        TextureResource& dstTexture = TransientResourcePool::GetTextureObject(copyInfo.DstTexture, frameIndex);
        VkBuffer srcBuffer = TransientResourcePool::GetBufferObject(copyInfo.SrcBuffer, frameIndex).Buffer;
        VkImage dstImage = dstTexture.Image;
        VkFormat dstFormat = GetVkImageFormat(dstTexture.TextureInfo.Format);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.SrcOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.TextureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             1, &bufferImageCopy);
    }
}

void RenderGraph::RecordPersistentBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferToTextureCopy>& copies = pass.persistentBufferToTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferToTextureCopy copyInfo = copies[i];

        TextureInfo& dstTexture = MemoryRegistry::GetTextureInfo(copyInfo.DstTexture);
        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(copyInfo.SrcBuffer).Buffer;
        VkImage dstImage = MemoryRegistry::GetTexture(copyInfo.DstTexture).Image;
        VkFormat dstFormat = GetVkImageFormat(dstTexture.Format);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.SrcOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.ArrayLayers
            },
            .imageOffset{copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             1, &bufferImageCopy);
    }
}

void RenderGraph::RecordTransientTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureToBufferCopy>& copies = pass.transientTextureToBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureToBufferCopy copyInfo = copies[i];

        TextureResource& srcTexture = TransientResourcePool::GetTextureObject(copyInfo.SrcTexture, frameIndex);
        VkImage srcImage = srcTexture.Image;
        VkBuffer dstBuffer = TransientResourcePool::GetBufferObject(copyInfo.DstBuffer, frameIndex).Buffer;
        VkFormat srcFormat = GetVkImageFormat(srcTexture.TextureInfo.Format);
        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.DstOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.TextureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x), 
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyImageToBuffer(cmdBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstBuffer, 1, &bufferImageCopy);
    }
}

void RenderGraph::RecordPersistentTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureToBufferCopy>& copies = pass.persistentTextureToBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureToBufferCopy copyInfo = copies[i];

        TextureInfo& srcTexture = MemoryRegistry::GetTextureInfo(copyInfo.SrcTexture);
        VkImage srcImage = MemoryRegistry::GetTexture(copyInfo.SrcTexture).Image;
        VkBuffer dstBuffer = MemoryRegistry::GetBuffer(copyInfo.DstBuffer).Buffer;
        VkFormat srcFormat = GetVkImageFormat(srcTexture.Format);
        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.DstOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.ArrayLayers
            },
            .imageOffset{copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x), 
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyImageToBuffer(cmdBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstBuffer, 1, &bufferImageCopy);
    }
}

void RenderGraph::RecordTransientPersistentBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferCopy>& copies = pass.transientPersistentBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferCopy copyInfo = copies[i];

        VkBuffer srcBuffer = TransientResourcePool::GetBufferObject(copyInfo.SrcBuffer, frameIndex).Buffer;
        VkBuffer dstBuffer = MemoryRegistry::GetBuffer(copyInfo.DstBuffer).Buffer;

        VkBufferCopy bufferCopy
        {
            .srcOffset = copyInfo.SrcOffset,
            .dstOffset = copyInfo.DstOffset,
            .size = copyInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    }
}

void RenderGraph::RecordPersistentTransientBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferCopy>& copies = pass.persistentTransientBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferCopy copyInfo = copies[i];

        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(copyInfo.SrcBuffer).Buffer;
        VkBuffer dstBuffer = TransientResourcePool::GetBufferObject(copyInfo.DstBuffer, frameIndex).Buffer;

        VkBufferCopy bufferCopy
        {
            .srcOffset = copyInfo.SrcOffset,
            .dstOffset = copyInfo.DstOffset,
            .size = copyInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    }
}

void RenderGraph::RecordTransientPersistentTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureCopy>& copies = pass.transientPersistentTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureCopy copyInfo = copies[i];

        TextureResource& srcTexture = TransientResourcePool::GetTextureObject(copyInfo.SrcTexture, frameIndex);
        TextureInfo& textureInfo = MemoryRegistry::GetTextureInfo(copyInfo.DstTexture);

        VkImage srcImage = srcTexture.Image;
        VkImage dstImage = MemoryRegistry::GetTexture(copyInfo.DstTexture).Image;

        VkFormat srcFormat = GetVkImageFormat(srcTexture.TextureInfo.Format);
        VkFormat dstFormat = GetVkImageFormat(textureInfo.Format);

        // CORRETTO: typo srcAspcetMask -> srcAspectMask
        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkImageCopy imageCopy
        {
            .srcSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.TextureInfo.ArrayLayers
            },
            .srcOffset {copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},

            .dstSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = textureInfo.ArrayLayers
            },
            .dstOffset {copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},

            .extent {
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y),
                static_cast<uint32_t>(copyInfo.Extent.z)
            }
        };

        vkCmdCopyImage(cmdBuffer, 
            srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }
}

void RenderGraph::RecordPersistentTransientTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureCopy>& copies = pass.persistentTransientTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureCopy copyInfo = copies[i];

        TextureInfo& textureInfo = MemoryRegistry::GetTextureInfo(copyInfo.SrcTexture);
        TextureResource& dstTexture = TransientResourcePool::GetTextureObject(copyInfo.DstTexture, frameIndex);

        VkImage srcImage = MemoryRegistry::GetTexture(copyInfo.SrcTexture).Image;
        VkImage dstImage = dstTexture.Image;

        VkFormat srcFormat = GetVkImageFormat(textureInfo.Format);
        VkFormat dstFormat = GetVkImageFormat(dstTexture.TextureInfo.Format);

        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkImageCopy imageCopy
        {
            .srcSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = textureInfo.ArrayLayers
            },
            .srcOffset {copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},

            .dstSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.TextureInfo.ArrayLayers
            },
            .dstOffset {copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},

            .extent {
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y),
                static_cast<uint32_t>(copyInfo.Extent.z)
            }
        };

        vkCmdCopyImage(cmdBuffer, 
            srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }
}

void RenderGraph::RecordTransientPersistentBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferToTextureCopy>& copies = pass.transientPersistentBufferToTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferToTextureCopy copyInfo = copies[i];

        TextureInfo& textureInfo = MemoryRegistry::GetTextureInfo(copyInfo.DstTexture);
        VkBuffer srcBuffer = TransientResourcePool::GetBufferObject(copyInfo.SrcBuffer, frameIndex).Buffer;
        VkImage dstImage = MemoryRegistry::GetTexture(copyInfo.DstTexture).Image;
        VkFormat dstFormat = GetVkImageFormat(textureInfo.Format);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.SrcOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = textureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             1, &bufferImageCopy);
    }
}

void RenderGraph::RecordPersistentTransientBufferToTextureCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferToTextureCopy>& copies = pass.persistentTransientBufferToTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferToTextureCopy copyInfo = copies[i];

        TextureResource& dstTexture = TransientResourcePool::GetTextureObject(copyInfo.DstTexture, frameIndex);
        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(copyInfo.SrcBuffer).Buffer;
        VkImage dstImage = dstTexture.Image;
        VkFormat dstFormat = GetVkImageFormat(dstTexture.TextureInfo.Format);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.SrcOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.TextureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
             1, &bufferImageCopy);
    }
}

void RenderGraph::RecordTransientPersistentTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureToBufferCopy>& copies = pass.transientPersistentTextureToBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureToBufferCopy copyInfo = copies[i];

        TextureResource& srcTexture = TransientResourcePool::GetTextureObject(copyInfo.SrcTexture, frameIndex);
        VkImage srcImage = srcTexture.Image;
        VkBuffer dstBuffer = MemoryRegistry::GetBuffer(copyInfo.DstBuffer).Buffer;
        VkFormat srcFormat = GetVkImageFormat(srcTexture.TextureInfo.Format);
        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.DstOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = srcTexture.TextureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x), 
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyImageToBuffer(cmdBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstBuffer, 1, &bufferImageCopy);
    }
}

void RenderGraph::RecordPersistentTransientTextureToBufferCopy(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureToBufferCopy>& copies = pass.persistentTransientTextureToBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureToBufferCopy copyInfo = copies[i];

        TextureInfo& textureInfo = MemoryRegistry::GetTextureInfo(copyInfo.SrcTexture);
        VkImage srcImage = MemoryRegistry::GetTexture(copyInfo.SrcTexture).Image;
        VkBuffer dstBuffer = TransientResourcePool::GetBufferObject(copyInfo.DstBuffer, frameIndex).Buffer;
        VkFormat srcFormat = GetVkImageFormat(textureInfo.Format);
        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.DstOffset,
            .bufferRowLength = copyInfo.BufferRowLength,
            .bufferImageHeight = copyInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = textureInfo.ArrayLayers
            },
            .imageOffset{copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x), 
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyImageToBuffer(cmdBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstBuffer, 1, &bufferImageCopy);
    }
}

void RenderGraph::RecordTransientBufferUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferUpload>& uploads = pass.transientBufferUploads;

    for(uint32_t i = 0; i < uploads.size(); i++)
    {
        BufferUpload uploadInfo = uploads[i];

        BufferObject& srcBuffer = MemoryRegistry::GetBuffer(uploadInfo.SrcBufferId);
        BufferObject& dstBuffer = MemoryRegistry::GetBuffer(uploadInfo.DstBuffer);

        VkBufferCopy bufferCopy
        {
            .srcOffset = 0,
            .dstOffset = uploadInfo.DstOffset,
            .size = uploadInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer.Buffer, dstBuffer.Buffer, 1, &bufferCopy);

        MemoryRegistry::DestroyBuffer(uploadInfo.SrcBufferId);
    }
}

void RenderGraph::RecordPersistentBufferUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferUpload>& uploads = pass.persistentBufferUploads;

    for(uint32_t i = 0; i < uploads.size(); i++)
    {
        BufferUpload uploadInfo = uploads[i];

        BufferObject& srcBuffer = MemoryRegistry::GetBuffer(uploadInfo.SrcBufferId);
        BufferObject& dstBuffer = MemoryRegistry::GetBuffer(uploadInfo.DstBuffer);

        VkBufferCopy bufferCopy
        {
            .srcOffset = 0,
            .dstOffset = uploadInfo.DstOffset,
            .size = uploadInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer.Buffer, dstBuffer.Buffer, 1, &bufferCopy);

        MemoryRegistry::DestroyBuffer(uploadInfo.SrcBufferId);
    }
}

void RenderGraph::RecordTransientTextureUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureUpload>& uploads = pass.transientTextureUploads;

    for(uint32_t i = 0; i < uploads.size(); i++)
    {
        TextureUpload uploadInfo = uploads[i];

        TextureResource& dstTexture = TransientResourcePool::GetTextureObject(uploadInfo.DstTexture, frameIndex);

        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(uploadInfo.SrcBufferId).Buffer;
        VkImage dstImage = MemoryRegistry::GetTexture(uploadInfo.DstTexture).Image;

        VkFormat format = GetVkImageFormat(dstTexture.TextureInfo.Format);

        VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = 0,
            .bufferRowLength = uploadInfo.BufferRowLength,
            .bufferImageHeight = uploadInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = aspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.TextureInfo.ArrayLayers,
            },
            .imageOffset {uploadInfo.DstOffset.x, uploadInfo.DstOffset.y, uploadInfo.DstOffset.z},
            .imageExtent {
                static_cast<uint32_t>(uploadInfo.Extent.x), 
                static_cast<uint32_t>(uploadInfo.Extent.y), 
                static_cast<uint32_t>(uploadInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &bufferImageCopy);

        MemoryRegistry::DestroyBuffer(uploadInfo.SrcBufferId);
    }
}

void RenderGraph::RecordPersistentTextureUpload(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureUpload>& uploads = pass.persistentTextureUploads;

    for(uint32_t i = 0; i < uploads.size(); i++)
    {
        TextureUpload uploadInfo = uploads[i];

        TextureInfo& dstTexture = MemoryRegistry::GetTextureInfo(uploadInfo.DstTexture);

        VkBuffer srcBuffer = MemoryRegistry::GetBuffer(uploadInfo.SrcBufferId).Buffer;
        VkImage dstImage = MemoryRegistry::GetTexture(uploadInfo.DstTexture).Image;

        VkFormat format = GetVkImageFormat(dstTexture.Format);

        VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = 0,
            .bufferRowLength = uploadInfo.BufferRowLength,
            .bufferImageHeight = uploadInfo.BufferHeightLength,
            .imageSubresource
            {
                .aspectMask = aspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = dstTexture.ArrayLayers,
            },
            .imageOffset {uploadInfo.DstOffset.x, uploadInfo.DstOffset.y, uploadInfo.DstOffset.z},
            .imageExtent {
                static_cast<uint32_t>(uploadInfo.Extent.x), 
                static_cast<uint32_t>(uploadInfo.Extent.y), 
                static_cast<uint32_t>(uploadInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &bufferImageCopy);

        MemoryRegistry::DestroyBuffer(uploadInfo.SrcBufferId);
    }
}

void RenderGraph::RecordDrawCalls(VkCommandBuffer cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<std::pair<TransientTextureHandle, Usage>>& textures = pass.transientTextures;
    std::vector<std::pair<TransientTextureHandle, LoadStoreOp>>& loadStoreOps = pass.loadStoreOps;
    std::vector<DrawCall>& drawCalls = pass.drawCalls;

    std::array<std::byte, 128> currentPushConstantData {};
    bool isFirstDrawCall = true;
    
    // No draw calls
    if(drawCalls.empty()) { return; }

    bool useColorTarget = false;
    bool useDepthTarget = false;
    bool useStencilTarget = false;

    std::vector<TransientTextureHandle> colorTargets;
    TransientTextureHandle depthTarget;
    TransientTextureHandle stencilTarget;

    // Get color, depth and stencil targets
    for(uint32_t i = 0; i < textures.size(); i++)
    {
        std::pair<TransientTextureHandle, Usage> data = textures[i];

        switch (data.second)
        {
            case (Usage::COLOR_ATTACHMENT) : 
                useColorTarget = true;
                colorTargets.push_back(data.first);
                break;
            case (Usage::DEPTH_STENCIL) :
                useDepthTarget = true;
                useStencilTarget = true;
                depthTarget = data.first;
                stencilTarget = data.first;
                break;
            case(Usage::DEPTH) :
                useDepthTarget = true;
                depthTarget = data.first;
                break;
            case(Usage::STENCIL) :
                useStencilTarget = true;
                stencilTarget = data.first;
                break;
            default :
                continue;
        }
    }

    // No target to write onto
    if(useColorTarget == false && useDepthTarget == false && useStencilTarget == false) { return; }

    std::vector<VkRenderingAttachmentInfoKHR> colorsAttachmentInfo;
    VkRenderingAttachmentInfoKHR depthAttachmentInfo{};
    VkRenderingAttachmentInfoKHR stencilAttachmentInfo{};

    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;
    for(uint32_t i = 0; i < colorTargets.size(); i++)
    {
        TransientTextureHandle handle = colorTargets[i];

        uint32_t index = transientTextureHandleToIndex[handle.Id];

        uint32_t _width = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Width;
        uint32_t _height = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);

        VkImageView imageView = TransientResourcePool::GetTextureObject(index, frameIndex).ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != handle.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfoKHR attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue {.color{.float32{
                loadStoreOp.clearColor.x, 
                loadStoreOp.clearColor.y, 
                loadStoreOp.clearColor.z, 
                1.0}}}
        };

        colorsAttachmentInfo.push_back(attachment);
    }

    if(useDepthTarget)
    {
        uint32_t index = transientTextureHandleToIndex[depthTarget.Id];
        uint32_t _width = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Width;
        uint32_t _height = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);


        VkImageView imageView = TransientResourcePool::GetTextureObject(index, frameIndex).ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != depthTarget.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfoKHR attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue {.depthStencil{.depth = loadStoreOp.clearDepth}}
        };

        depthAttachmentInfo = attachment;
    }

    if(useStencilTarget)
    {
        uint32_t index = transientTextureHandleToIndex[stencilTarget.Id];
        uint32_t _width = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Width;
        uint32_t _height = TransientResourcePool::GetTextureObject(index, frameIndex).TextureInfo.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);

        VkImageView imageView = TransientResourcePool::GetTextureObject(index, frameIndex).ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != stencilTarget.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfoKHR attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue {.depthStencil{.stencil = loadStoreOp.clearStencil}}
        };

        stencilAttachmentInfo = attachment;
    }

    // Invalid resolution
    if(width == UINT32_MAX || height == UINT32_MAX) { return; }

    VkRenderingInfoKHR renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = width, .height = height}
        },
        .layerCount = 1,
        .colorAttachmentCount = static_cast<uint32_t>(colorsAttachmentInfo.size()),
        .pColorAttachments = colorsAttachmentInfo.empty() ? nullptr : colorsAttachmentInfo.data(),
        .pDepthAttachment = useDepthTarget ? &depthAttachmentInfo : nullptr,
        .pStencilAttachment = useStencilTarget ? &stencilAttachmentInfo : nullptr
    };

    vkCmdBeginRenderingKHR(cmdBuffer, &renderInfo);
    {

        VkViewport viewport
        {
            .x = 0, .y = static_cast<float>(height),
            .width = static_cast<float>(width),
            .height =  -static_cast<float>(height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = width, .height = height}
        };

        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

        for(uint32_t i = 0; i < drawCalls.size(); i++)
        {
            DrawCall drawCall = drawCalls[i];

            uint32_t pushConstantSize = drawCall.Size.ToBytes();
            uint32_t pushConstantOffset = drawCall.Offset.ToBytes();
            ShaderHandle shaderHandle = drawCall.MaterialHandle.GetShader();
            GraphicsShaderObject shader = ShaderRegistry::GetShaderObject(shaderHandle);

            vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.Pipeline);

            // --- Push Constant ---
            std::byte* currentData = currentPushConstantData.data() + pushConstantOffset;
            const std::byte* newData = drawCall.PushCostant.data() + pushConstantOffset;

            bool dataChanged = std::memcmp(currentData, newData, pushConstantSize) != 0;

            if(dataChanged || isFirstDrawCall)
            {
                vkCmdPushConstants(
                    cmdBuffer, 
                    PipelineBuilder::GetGraphicsPipelineLayout(), 
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                    pushConstantOffset,
                    pushConstantSize,
                    newData
                );

                std::memcpy(
                    currentData,
                    newData,
                    pushConstantSize
                );

                isFirstDrawCall = false;
            }

            // --- Draw ---
            vkCmdDraw(cmdBuffer, drawCall.VertexShaderInvocations, drawCall.InstanceCount, 0, 0);
        }
    }
    vkCmdEndRenderingKHR(cmdBuffer);
}

void RenderGraph::RecordSwapchainDrawingPass(VkCommandBuffer cmdBuffer, uint32_t frameIndex, uint32_t swapchainImageIndex)
{
    VkImage swapchainImage = GraphicsCore::Swapchain.swapchainImages[swapchainImageIndex];
    VkImageView swapchainImageView = GraphicsCore::Swapchain.swapchainImageViews[swapchainImageIndex];

    std::vector<VkImageMemoryBarrier2KHR> barriers
    {
        VkImageMemoryBarrier2KHR
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            .srcAccessMask = VK_ACCESS_2_NONE_KHR,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = swapchainImage,
            .subresourceRange
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }
    };

    uint32_t resourceIndex;
    if(isPresentTextureValid)
    {
        resourceIndex = transientTextureHandleToIndex[presentTexture.Id];

        TextureResource& colorTexture = TransientResourcePool::GetTextureObject(resourceIndex, frameIndex);

        TextureBarrierInfo srcBarrierInfo
        {
            .StageMask = VK_PIPELINE_STAGE_2_NONE,
            .AccessMask = VK_ACCESS_2_NONE,
            .Layout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        uint32_t offset = barriersOffsetPerTexture[resourceIndex];
        uint32_t size = barriersOffsetPerTexture[resourceIndex + 1] - offset;
        for(uint32_t i = 0; i < size; i++)
        {
            std::pair<TextureBarrierInfo, uint32_t>& barrierInfo = texturesBarriersInfo[i + offset];
            if(barrierInfo.first.TextureId != resourceIndex) { continue; }

            srcBarrierInfo = barrierInfo.first;
        }

        VkFormat format = GetVkImageFormat(colorTexture.TextureInfo.Format);
        VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

        VkImageMemoryBarrier2KHR colorBarrier
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
            .srcStageMask = srcBarrierInfo.StageMask,
            .srcAccessMask = srcBarrierInfo.AccessMask,
            .dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT_KHR,
            .dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT_KHR,
            .oldLayout = srcBarrierInfo.Layout,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = colorTexture.Image,
            .subresourceRange
            {
                .aspectMask = aspectMask,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        barriers.push_back(colorBarrier);
    }

    VkDependencyInfoKHR drawDep
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .imageMemoryBarrierCount = static_cast<uint32_t>(barriers.size()),
        .pImageMemoryBarriers = barriers.data()
    };

    vkCmdPipelineBarrier2KHR(cmdBuffer, &drawDep);

    VkRenderingAttachmentInfoKHR colorAttachment
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = swapchainImageView,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue {.color{.float32{
            0, 
            0, 
            0, 
            1.0}}}
    };

    VkRenderingInfoKHR renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = GraphicsCore::Swapchain.Width, .height = GraphicsCore::Swapchain.Height}
        },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment
    };

    vkCmdBeginRenderingKHR(cmdBuffer, &renderInfo);
    if(presentTexture.Id != UINT32_MAX)
    {
        VkViewport viewport
        {
            .x = 0, .y = 0,
            .width = static_cast<float>(GraphicsCore::Swapchain.Width),
            .height =  static_cast<float>(GraphicsCore::Swapchain.Height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        VkRect2D scissor
        {
            .offset {.x = 0, .y = 0},
            .extent {.width = GraphicsCore::Swapchain.Width, .height = GraphicsCore::Swapchain.Height}
        };

        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
        
        GraphicsShaderObject shaderObject = ShaderRegistry::GetShaderObject(GraphicsCore::Swapchain.shader);

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shaderObject.Pipeline);

        struct PushConstant{uint32_t textureId; uint32_t samplerId;} pushConstant;
        
        pushConstant.textureId = presentTexture.Id;
        pushConstant.samplerId = GraphicsCore::Swapchain.sampler.Id;

        vkCmdPushConstants(
            cmdBuffer, 
            PipelineBuilder::GetGraphicsPipelineLayout(), 
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
            0, 
            sizeof(PushConstant), 
            &pushConstant
        );

        vkCmdDraw(cmdBuffer, 6, 1, 0, 0);
    }
    vkCmdEndRenderingKHR(cmdBuffer);

    VkImageMemoryBarrier2KHR presentBarrier
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2_KHR,
        .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR,
        .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT_KHR,
        .dstStageMask = VK_PIPELINE_STAGE_2_NONE_KHR,
        .dstAccessMask = VK_ACCESS_2_NONE_KHR,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapchainImage,
        .subresourceRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    VkDependencyInfoKHR presentDep
    {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &presentBarrier
    };

    vkCmdPipelineBarrier2KHR(cmdBuffer, &presentDep);
}

RenderGraph::TextureBarrierInfo RenderGraph::GetFirstTextureBarrierInfo(const uint32_t newAllocIndex, const uint64_t newAllocOffset, const uint64_t newAllocSize)
{
    TextureBarrierInfo barrier
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
        .Layout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    uint64_t start = newAllocOffset;
    uint64_t end = newAllocOffset + newAllocSize;

    MemorySlot newMemorySlot{start, end, newAllocIndex};

    auto it = std::lower_bound(virtualMemorySlots.begin(), virtualMemorySlots.end(), newMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
        return a.End <= b.Start;
    });

    uint32_t firstSlotIndex = it - virtualMemorySlots.begin();

    uint32_t slotsCount = 0;
    for(; it != virtualMemorySlots.end(); it++)
    {
        if(it->Start >= newMemorySlot.End) { break; }

        uint32_t lastBarrierIndex = barriersOffsetPerTexture[it->ResourceIndex + 1] - 1;

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
            MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceIndex};

            fragments[fragmentsCount++] = startSlot;
        }

        fragments[fragmentsCount++] = newMemorySlot;

        if(strideEnd > 0)
        {
            // Free memory at the end of the slot
            MemorySlot endSlot{newMemorySlot.End, newMemorySlot.End + strideEnd, lastSlot.ResourceIndex};

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

RenderGraph::BufferBarrierInfo RenderGraph::GetFirstBufferBarrierInfo(const uint32_t newAllocIndex, const uint64_t newAllocOffset, const uint64_t newAllocSize)
{
    BufferBarrierInfo barrier
    {
        .StageMask = VK_PIPELINE_STAGE_2_NONE,
        .AccessMask = VK_ACCESS_2_NONE,
    };

    uint64_t start = newAllocOffset;
    uint64_t end = newAllocOffset + newAllocSize;

    MemorySlot newMemorySlot{start, end, newAllocIndex};

    auto it = std::lower_bound(virtualMemorySlots.begin(), virtualMemorySlots.end(), newMemorySlot, [](const MemorySlot& a, const MemorySlot& b){
        return a.End <= b.Start;
    });

    uint32_t firstSlotIndex = it - virtualMemorySlots.begin();

    uint32_t slotsCount = 0;
    for(; it != virtualMemorySlots.end(); it++)
    {
        if(it->Start >= newMemorySlot.End) { break; }

        uint32_t lastBarrierIndex = barriersOffsetPerBuffer[it->ResourceIndex + 1] - 1;

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
            MemorySlot startSlot{firstSlot.Start, firstSlot.Start + strideStart, firstSlot.ResourceIndex};

            fragments[fragmentsCount++] = startSlot;
        }

        fragments[fragmentsCount++] = newMemorySlot;

        if(strideEnd > 0)
        {
            // Free memory at the end of the slot
            MemorySlot endSlot{newMemorySlot.End, newMemorySlot.End + strideEnd, lastSlot.ResourceIndex};

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

TransientTextureHandle RenderGraph::RequestTransientTexture1D(TransientTextureInfo1D textureInfo)
{
    TransientTextureHandle handle = ResourceRegistry::RequestTransientTextureSlot();

    TextureInfo data
    {
        .TextureType = TextureType::TEXTURE_1D,
        .Width = textureInfo.Width, 
        .Height = 1,
        .Depth = 1,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = 0
    };

    transientRequestedTextures.push_back(data);
    transientRequestedTextureHandles.push_back(handle);

    uint32_t index = transientRequestedTextures.size() - 1;

    if(transientTextureHandleToIndex.size() <= handle.Id)
    {
        transientTextureHandleToIndex.resize(handle.Id + 1);
    }

    transientTextureHandleToIndex[handle.Id] = index;

    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture2D(TransientTextureInfo2D textureInfo)
{
    TransientTextureHandle handle = ResourceRegistry::RequestTransientTextureSlot();
    TextureInfo data
    {
        .TextureType = TextureType::TEXTURE_2D,
        .Width = textureInfo.Width, 
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = 0
    };

    transientRequestedTextures.push_back(data);
    transientRequestedTextureHandles.push_back(handle);

    uint32_t index = transientRequestedTextures.size() - 1;

    if(transientTextureHandleToIndex.size() <= handle.Id)
    {
        transientTextureHandleToIndex.resize(handle.Id + 1);
    }

    transientTextureHandleToIndex[handle.Id] = index;

    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture3D(TransientTextureInfo3D textureInfo)
{
    TransientTextureHandle handle = ResourceRegistry::RequestTransientTextureSlot();
    TextureInfo data
    {
        .TextureType = TextureType::TEXTURE_3D,
        .Width = textureInfo.Width, 
        .Height = textureInfo.Height,
        .Depth = textureInfo.Depth,
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = 0
    };

    transientRequestedTextures.push_back(data);
    transientRequestedTextureHandles.push_back(handle);

    uint32_t index = transientRequestedTextures.size() - 1;

    if(transientTextureHandleToIndex.size() <= handle.Id)
    {
        transientTextureHandleToIndex.resize(handle.Id + 1);
    }

    transientTextureHandleToIndex[handle.Id] = index;

    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTextureCube(TransientTextureInfo2D textureInfo)
{
    TransientTextureHandle handle = ResourceRegistry::RequestTransientTextureSlot();
    TextureInfo data
    {
        .TextureType = TextureType::TEXTURE_3D,
        .Width = textureInfo.Width, 
        .Height = textureInfo.Height,
        .Depth = 1,
        .ArrayLayers = 6,
        .MipLevels = 1,
        .Format = textureInfo.Format,
        .Usage = 0
    };

    transientRequestedTextures.push_back(data);
    transientRequestedTextureHandles.push_back(handle);

    uint32_t index = transientRequestedTextures.size() - 1;

    if(transientTextureHandleToIndex.size() <= handle.Id)
    {
        transientTextureHandleToIndex.resize(handle.Id + 1);
    }

    transientTextureHandleToIndex[handle.Id] = index;

    return handle;
}

TransientBufferHandle RenderGraph::RequestTransientBuffer(uint64_t size)
{
    TransientBufferHandle handle = ResourceRegistry::RequestTransientBufferSlot();

    BufferInfo data
    {
        .Size = size,
        .Usage = 0
    };

    transientRequestedBuffers.push_back(data);
    transientRequestedBufferHandles.push_back(handle);

    uint32_t index = transientRequestedBuffers.size() - 1;

    if(transientBufferHandleToIndex.size() <= handle.Id)
    {
        transientBufferHandleToIndex.resize(handle.Id + 1);
    }

    transientBufferHandleToIndex[handle.Id] = index;

    return handle;
}

void RenderGraph::AddTextureBucketPasses(uint32_t passesCount)
{
    texturesBucketPasses.push_back(std::vector<TexturesBucketPass>(passesCount));
}

void RenderGraph::AddBufferBucketPasses(uint32_t passesCount)
{
    buffersBucketPasses.push_back(std::vector<BuffersBucketPass>(passesCount));
}

void RenderGraph::AddPass(GraphicsPass& pass)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),

        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures(),

        .loadStoreOps = pass.GetLoadStoreOperations(),
        .drawCalls = pass.GetDrawCalls()
    };

    passes.push_back(data);
}

void RenderGraph::AddPass(TransferPass& pass)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),

        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures(),

        .transientBufferCopies = pass.GetTransientBufferCopies(),
        .transientTextureCopies = pass.GetTransientTextureCopies(),
        .transientBufferToTextureCopies = pass.GetTransientBufferToTextureCopies(),
        .transientTextureToBufferCopies = pass.GetTransientTextureToBufferCopies(),

        .persistentBufferCopies = pass.GetPersistentBufferCopies(),
        .persistentTextureCopies = pass.GetPersistentTextureCopies(),
        .persistentBufferToTextureCopies = pass.GetPersistentBufferToTextureCopies(),
        .persistentTextureToBufferCopies = pass.GetPersistentTextureToBufferCopies(),

        .transientPersistentBufferCopies = pass.GetTransientPersistentBufferCopies(),
        .persistentTransientBufferCopies = pass.GetPersistentTransientBufferCopies(),
        .transientPersistentTextureCopies = pass.GetTransientPersistentTextureCopies(),
        .persistentTransientTextureCopies = pass.GetPersistentTransientTextureCopies(),

        .transientPersistentBufferToTextureCopies = pass.GetTransientPersistentBufferToTextureCopies(),
        .persistentTransientBufferToTextureCopies = pass.GetPersistentTransientBufferToTextureCopies(),
        .transientPersistentTextureToBufferCopies = pass.GetTransientPersistentTextureToBufferCopies(),
        .persistentTransientTextureToBufferCopies = pass.GetPersistentTransientTextureToBufferCopies(),

        .transientBufferUploads = pass.GetTransientBufferUploads(),
        .transientTextureUploads = pass.GetTransientTextureUploads(),
        .persistentBufferUploads = pass.GetPersistentBufferUploads(),
        .persistentTextureUploads = pass.GetPersistentTextureUploads()
    };

    passes.push_back(data);
}

void RenderGraph::AddPass(ComputePass& pass)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),
        
        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures()

        // To add compute
    };
    passes.push_back(data);
}

void RenderGraph::AddPass(GraphicsPass& pass, uint32_t index)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),

        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures(),

        .loadStoreOps = pass.GetLoadStoreOperations(),
        .drawCalls = pass.GetDrawCalls()
    };

    passes.insert(passes.cbegin() + index, data);
}

void RenderGraph::AddPass(TransferPass& pass, uint32_t index)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),

        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures(),

        .transientBufferCopies = pass.GetTransientBufferCopies(),
        .transientTextureCopies = pass.GetTransientTextureCopies(),
        .transientBufferToTextureCopies = pass.GetTransientBufferToTextureCopies(),
        .transientTextureToBufferCopies = pass.GetTransientTextureToBufferCopies(),

        .persistentBufferCopies = pass.GetPersistentBufferCopies(),
        .persistentTextureCopies = pass.GetPersistentTextureCopies(),
        .persistentBufferToTextureCopies = pass.GetPersistentBufferToTextureCopies(),
        .persistentTextureToBufferCopies = pass.GetPersistentTextureToBufferCopies(),

        .transientPersistentBufferCopies = pass.GetTransientPersistentBufferCopies(),
        .persistentTransientBufferCopies = pass.GetPersistentTransientBufferCopies(),
        .transientPersistentTextureCopies = pass.GetTransientPersistentTextureCopies(),
        .persistentTransientTextureCopies = pass.GetPersistentTransientTextureCopies(),

        .transientPersistentBufferToTextureCopies = pass.GetTransientPersistentBufferToTextureCopies(),
        .persistentTransientBufferToTextureCopies = pass.GetPersistentTransientBufferToTextureCopies(),
        .transientPersistentTextureToBufferCopies = pass.GetTransientPersistentTextureToBufferCopies(),
        .persistentTransientTextureToBufferCopies = pass.GetPersistentTransientTextureToBufferCopies(),

        .transientBufferUploads = pass.GetTransientBufferUploads(),
        .transientTextureUploads = pass.GetTransientTextureUploads(),
        .persistentBufferUploads = pass.GetPersistentBufferUploads(),
        .persistentTextureUploads = pass.GetPersistentTextureUploads()
    };

    passes.insert(passes.cbegin() + index, data);
}

void RenderGraph::AddPass(ComputePass& pass, uint32_t index)
{
    Pass data
    {
        .transientBuffers = pass.GetTransientBuffers(),
        .transientTextures = pass.GetTransientTextures(),
        
        .persistentBuffers = pass.GetPersistentBuffers(),
        .persistentTextures = pass.GetPersistentTextures()

        // To add compute
    };

    passes.insert(passes.cbegin() + index, data);
}

void RenderGraph::SetPresentTexture(TransientTextureHandle handle)
{
    presentTexture = handle;
}