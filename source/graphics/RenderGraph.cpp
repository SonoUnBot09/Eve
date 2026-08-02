#include "Eve/graphics/Mesh.hpp"
#include "Eve/graphics/PassModule.hpp"
#include "Eve/graphics/Texture.hpp"
#include "GraphicsCore.hpp"
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include "builders/ShaderObject.hpp"
#include "graphics/helpers/VulkanMapping.hpp"
#include <graphics/registers/MeshRegistry.hpp>
#include <cstdint>
#include <graphics/RenderGraph.hpp>
#include <graphics/registers/ShaderRegistry.hpp>
#include <graphics/registers/MeshRegistry.hpp>
#include <graphics/ResourceMapper.hpp>

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

            case (Usage::DEPTH_STENCIL) :
                return TextureUsage::USAGE_DEPTH_STENCIL_ATTACHMENT; 

            case (Usage::DEPTH) :
                return TextureUsage::USAGE_DEPTH_STENCIL_ATTACHMENT; 

            case (Usage::STENCIL) :
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
                    VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::DEPTH_STENCIL) :
                return
                {
                    VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                    VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::DEPTH) :
                return
                {
                    VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                    VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
                };

            case(Usage::STENCIL) :
                return
                {
                    VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
                    VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL
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

    void CreateTransientTexture(TextureInfo textureInfo, VkImage& image, VkImageView& imageView)
    {

        VkImageType imageType = VK_IMAGE_TYPE_2D;
        VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_2D;
        if(textureInfo.Data.Depth > 1)
        {
            imageType = VK_IMAGE_TYPE_3D;
            imageViewType = VK_IMAGE_VIEW_TYPE_3D;
        }
        else if(textureInfo.Data.Height == 1)
        {
            imageType = VK_IMAGE_TYPE_1D;
            imageViewType = VK_IMAGE_VIEW_TYPE_1D;
        }

        VkFormat format = GetVkImageFormat(textureInfo.Data.Format);
        VkImageCreateInfo imageCI
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = imageType,
            .format = format,
            .extent {.width = textureInfo.Data.Width, .height = textureInfo.Data.Height, .depth = textureInfo.Data.Depth},
            .mipLevels = textureInfo.Data.MipLevels,
            .arrayLayers = textureInfo.Data.ArrayLayers,
            .samples = GetVkImageSamplesCount(textureInfo.Data.Sample),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = GetVkImageUsage(textureInfo.Data.Usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
        };

        VmaAllocationCreateInfo imageAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        vkCreateImage(GraphicsCore::Context.Device, &imageCI, nullptr, &image);
        
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
                .levelCount = textureInfo.Data.MipLevels,
                .baseArrayLayer = 0,
                .layerCount = textureInfo.Data.ArrayLayers
            }
        };

        vkCreateImageView(GraphicsCore::Context.Device, &imageViewCI, nullptr, &imageView);
    }

    void CreateTransientBuffer(BufferInfo bufferInfo, VkBuffer buffer)
    {
        VkBufferCreateInfo bufferCI
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = bufferInfo.Data.Size,
            .usage = GetVkBufferUsage(bufferInfo.Data.Usage) | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VmaAllocationCreateInfo bufferAllocInfo
        {
            .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        };

        vkCreateBuffer(GraphicsCore::Context.Device, &bufferCI, nullptr, &buffer);
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

    bool IsTextureBarrierNeeded(RenderGraph::PersistentTextureState src, RenderGraph::TextureBarrierInfo dst, Usage newUsage)
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

    bool IsBufferBarrierNeeded(RenderGraph::PersistentBufferState src, RenderGraph::BufferBarrierInfo dst, Usage newUsage)
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

bool RenderGraph::CompileGraph(VkCommandBuffer& cmdBuffer, uint32_t frameIndex)
{
    UpdateTexturesPool(frameIndex);
    UpdateBuffersPool(frameIndex);

    transientTextures[frameIndex].clear();
    transientBuffers[frameIndex].clear();

    uint32_t transientTexturesCount = transientRequestedTextures.size();
    uint32_t transientBuffersCount = transientRequestedBuffers.size();
    uint32_t passesCount = passes.size();

    for(uint32_t bucketIndex = 0; bucketIndex < texturesMemoryTypeIndicies.size(); bucketIndex++)
    {
        texturesBucketPasses[bucketIndex].resize(passesCount);
    }
    for(uint32_t bucketIndex = 0; bucketIndex < buffersMemoryTypeIndicies.size(); bucketIndex++)
    {
        buffersBucketPasses[bucketIndex].resize(passesCount);
    }

    // Calculate first and last passes
    // Calculate resources usage
    // Calculate resource memory info
    uint32_t texturesBarriersOffset = 0;
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

        Usage oldUsage = static_cast<Usage>(0);
        TextureUsage usage = static_cast<TextureUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientTextureHandle, Usage>>& textures = passes[passIndex].transientTextures;
            for(uint32_t i = 0; i < textures.size(); i++)
            {
                // For each textures in that pass grab the texture usage
                // and calculate the first and last passes
                if(textures[i].first.Id != textureId) { continue; }

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

        if(firstPassIndex == -1) { continue; }

        transientTextures[frameIndex][textureId].TextureInfo = transientRequestedTextures[textureId];
        transientTextures[frameIndex][textureId].TextureInfo.Data.Usage = usage;

        uint32_t bucketIndex = SetTextureMemoryInfo(frameIndex, textureId, passesCount);

        texturesBucketPasses[bucketIndex][firstPassIndex].TexturesToCreate.emplace_back(textureId);
        texturesBucketPasses[bucketIndex][lastPassIndex].TexturesToDestroy.emplace_back(textureId);
    }
    uint32_t buffersBarriersOffset = 0;
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

        Usage oldUsage = static_cast<Usage>(0);
        BufferUsage usage = static_cast<BufferUsage>(0);
        for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
        {
            // For each pass
            std::vector<std::pair<TransientBufferHandle, Usage>>& buffers = passes[passIndex].transientBuffers;
            for(uint32_t i = 0; i < buffers.size(); i++)
            {
                // For each buffer in that pass grab the texture usage
                // and calculate the first and last passes
                if(buffers[i].first.Id != bufferId) { continue; }

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

        transientBuffers[frameIndex][bufferId].BufferInfo = transientRequestedBuffers[bufferId];
        transientBuffers[frameIndex][bufferId].BufferInfo.Data.Usage = usage;

        uint32_t bucketIndex = SetBufferMemoryInfo(frameIndex, bufferId, passesCount);

        buffersBucketPasses[bucketIndex][firstPassIndex].BuffersToCreate.emplace_back(bufferId);
        buffersBucketPasses[bucketIndex][lastPassIndex].BuffersToDestroy.emplace_back(bufferId);

    }

    // Generate persistent texture and buffer barriers
    for(uint32_t passIndex = 0; passIndex < passesCount; passIndex++)
    {
        Pass& pass = passes[passIndex];
        std::vector<std::pair<TextureHandle, Usage>>& textures = pass.persistentTextures;
        std::vector<std::pair<BufferHandle, Usage>>& buffers = pass.persistentBuffers;

        // --- Textures ---
        for(uint32_t i = 0; i < textures.size(); i++)
        {
            uint32_t textureId = textures[i].first.Id;
            Usage newUsage = textures[i].second;

            PersistentTextureState& textureState = persistentTexturesState[textureId];
            TextureBarrierInfo dstBarrierInfo = CalculateTextureBarrierInfo(newUsage);

            bool barrier = IsTextureBarrierNeeded(textureState, dstBarrierInfo, newUsage);

            if(barrier)
            {
                dstBarrierInfo.TextureId = textureId;

                TextureBarrierInfo srcBarrierInfo
                {
                    .TextureId = textureId,
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
            uint32_t bufferId = buffers[i].first.Id;
            Usage newUsage = buffers[i].second;

            PersistentBufferState& bufferState = persistentBuffersState[bufferId];
            BufferBarrierInfo dstBarrierInfo = CalculateBufferBarrierInfo(newUsage);

            bool barrier = IsBufferBarrierNeeded(bufferState, dstBarrierInfo, newUsage);

            if(barrier)
            {
                dstBarrierInfo.BufferId = bufferId;

                BufferBarrierInfo srcBarrierInfo
                {
                    .BufferId = bufferId,
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
    vmaCreateVirtualBlock(&virtualBlockCI, &block);

    texturesVirtualAllocs.resize(transientTexturesCount);
    buffersVirtualAllocs.resize(transientBuffersCount);

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

        bool success = ResizeTextureMemoryPoolIfNeeded(bucketIndex, peakSize, peakAlignment);

        if(!success)
        {
            return false;
        }
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

        bool success = ResizeBufferMemoryPoolIfNeeded(bucketIndex, peakSize, peakAlignment);

        if(!success)
        {
            return false;
        }

    }

    texturesVirtualAllocs.clear();
    buffersVirtualAllocs.clear();

    vmaDestroyVirtualBlock(block);

    // Create/Reuse textures and buffers
    // Insert the barriers info in the right passes so the barriers can be created later
    for(uint32_t textureId = 0; textureId < transientTexturesCount; textureId++)
    {
        TextureResource& resource = transientTextures[frameIndex][textureId];

        auto it = std::lower_bound(texturesPool.begin(), texturesPool.end(), resource, [](const std::pair<TextureResource, bool>& a, const TextureResource& b)
        {
            return std::memcmp(&a.first.TextureInfo, &b.TextureInfo, sizeof(TextureInfo)) < 0;
        });

        bool found = false;
        for(; it != texturesPool.end(); it++)
        {
            if(std::memcmp(&it->first.TextureInfo, &resource.TextureInfo, sizeof(TextureInfo)) != 0)
            {
                break;
            }

            if(it->second == false)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            resource.Image = it->first.Image;
            resource.ImageView = it->first.ImageView;
            it->second = true;

            resource.PooledImage = true;
        }
        else 
        {
            CreateTransientTexture(resource.TextureInfo, resource.Image, resource.ImageView);

            MemoryBucket& memoryPool = texturesMemoryBucket[resource.MemoryInfo.BucketIndex];
            vmaBindImageMemory2(GraphicsCore::Context.Allocator, memoryPool.Allocation, 
                resource.TextureInfo.MemoryInfo.Offset, resource.Image, nullptr);

            resource.PooledImage = false;
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
    for(uint32_t bufferId = 0; bufferId < transientBuffersCount; bufferId++)
    {
        BufferResource& resource = transientBuffers[frameIndex][bufferId];

        auto it = std::lower_bound(buffersPool.begin(), buffersPool.end(), resource, [](const std::pair<BufferResource, bool>& a, const BufferResource& b)
        {
            return std::memcmp(&a.first.BufferInfo, &b.BufferInfo, sizeof(BufferInfo)) < 0;
        });

        bool found = false;
        for(; it != buffersPool.end(); it++)
        {
            if(std::memcmp(&it->first.BufferInfo, &resource.BufferInfo, sizeof(BufferInfo)) != 0)
            {
                break;
            }

            if(it->second == false)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            resource.Buffer = it->first.Buffer;
            it->second = true;

            resource.PooledBuffer = true;
        }
        else 
        {
            CreateTransientBuffer(resource.BufferInfo, resource.Buffer);

            MemoryBucket& memoryPool = buffersMemoryBucket[resource.MemoryInfo.BucketIndex];
            vmaBindBufferMemory2(GraphicsCore::Context.Allocator, memoryPool.Allocation, 
                resource.BufferInfo.MemoryInfo.Offset, resource.Buffer, nullptr);

            resource.PooledBuffer = false;
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

    ResourceMapper::MapResources(cmdBuffer, frameIndex);

    RecordCommands(frameIndex, cmdBuffer);

    transientRequestedTextures.clear();
    transientRequestedBuffers.clear();
    passes.clear();
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

    return true;
}

bool RenderGraph::RecordCommands(uint32_t frameIndex, VkCommandBuffer& cmdBuffer)
{
    std::vector<VkImageMemoryBarrier2> textureMemoryBarriers;
    std::vector<VkBufferMemoryBarrier2> bufferMemoryBarriers;

    uint32_t passCount = passes.size();
    for(uint32_t passIndex = 0; passIndex < passCount; passIndex++)
    {
        Pass& pass = passes[passIndex];
        
        std::vector<TextureBarrierInfoPair>& transientTextureBarriers = pass.transientTexturesBarriers;
        std::vector<BufferBarrierInfoPair>& transientBufferBarriers = pass.transientBuffersBarriers;
        std::vector<TextureBarrierInfoPair>& persistentTextureBarriers = pass.persistentTexturesBarriers;
        std::vector<BufferBarrierInfoPair>& persistentBufferBarriers = pass.persistentBuffersBarriers;

        // --- Record Transient Textures Barriers ---
        for(uint32_t i = 0; transientTextureBarriers.size(); i++)
        {
            TextureBarrierInfoPair barrierInfo = transientTextureBarriers[i];

            uint32_t textureIndex = barrierInfo.DstInfo.TextureId;

            VkImage image = transientTextures[frameIndex][textureIndex].Image;
            VkFormat format = GetVkImageFormat(transientTextures[frameIndex][textureIndex].TextureInfo.Data.Format);
            VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

            VkImageMemoryBarrier2 barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .oldLayout = barrierInfo.SrcInfo.Layout,
                .newLayout = barrierInfo.DstInfo.Layout,
                .image = image,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .subresourceRange
                {
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            textureMemoryBarriers.push_back(barrier);
        }

        // --- Record Transient Buffers Barriers ---
        for(uint32_t i = 0; transientBufferBarriers.size(); i++)
        {
            BufferBarrierInfoPair barrierInfo = transientBufferBarriers[i];

            uint32_t bufferIndex = barrierInfo.DstInfo.BufferId;

            VkBuffer buffer = transientBuffers[frameIndex][bufferIndex].Buffer;

            VkBufferMemoryBarrier2 barrier
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .buffer = buffer,
                .offset = 0,
                .size = VK_WHOLE_SIZE,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
            };

            bufferMemoryBarriers.push_back(barrier);
        }

        // --- Record Persistent Textures Barriers ---
        for(uint32_t i = 0; persistentTextureBarriers.size(); i++)
        {
            TextureBarrierInfoPair barrierInfo = persistentTextureBarriers[i];

            uint32_t textureId = barrierInfo.DstInfo.TextureId;

            VkImage image = MemoryRegistry::GetTexture(textureId).Image;
            VkFormat format = GetVkImageFormat(MemoryRegistry::GetTextureInfo(textureId).Data.Format);
            VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

            VkImageMemoryBarrier2 barrier
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .oldLayout = barrierInfo.SrcInfo.Layout,
                .newLayout = barrierInfo.DstInfo.Layout,
                .image = image,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .subresourceRange
                {
                    .aspectMask = aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };

            textureMemoryBarriers.push_back(barrier);
        }

        // --- Record Persistent Buffers Barriers ---
        for(uint32_t i = 0; persistentBufferBarriers.size(); i++)
        {
            BufferBarrierInfoPair barrierInfo = persistentBufferBarriers[i];

            uint32_t bufferId = barrierInfo.DstInfo.BufferId;

            VkBuffer buffer = MemoryRegistry::GetBuffer(bufferId).Buffer;

            VkBufferMemoryBarrier2 barrier
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
                .srcStageMask = barrierInfo.SrcInfo.StageMask,
                .srcAccessMask = barrierInfo.SrcInfo.AccessMask,
                .dstStageMask = barrierInfo.DstInfo.StageMask,
                .dstAccessMask = barrierInfo.DstInfo.AccessMask,
                .buffer = buffer,
                .offset = 0,
                .size = VK_WHOLE_SIZE,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
            };

            bufferMemoryBarriers.push_back(barrier);
        }

        VkDependencyInfo dependency
        {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .bufferMemoryBarrierCount = static_cast<uint32_t>(bufferMemoryBarriers.size()),
            .pBufferMemoryBarriers = bufferMemoryBarriers.data(),
            .imageMemoryBarrierCount = static_cast<uint32_t>(textureMemoryBarriers.size()),
            .pImageMemoryBarriers = textureMemoryBarriers.data()
        };

        if(bufferMemoryBarriers.size() > 0 || textureMemoryBarriers.size() > 0)
        {
            vkCmdPipelineBarrier2(cmdBuffer, &dependency);
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

        RecordDrawCalls(cmdBuffer, pass, frameIndex);

    }

    return true;
}

void RenderGraph::RecordTransientBufferCopy(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferCopy>& copies = pass.transientBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferCopy copyInfo = copies[i];

        VkBuffer srcBuffer = transientBuffers[frameIndex][copyInfo.SrcBuffer].Buffer;
        VkBuffer dstBuffer = transientBuffers[frameIndex][copyInfo.DstBuffer].Buffer;

        VkBufferCopy bufferCopy
        {
            .srcOffset = copyInfo.SrcOffset,
            .dstOffset = copyInfo.DstOffset,
            .size = copyInfo.Size
        };

        vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    }
}
void RenderGraph::RecordTransientTextureCopy(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureCopy>& copies = pass.transientTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureCopy copyInfo = copies[i];

        VkImage srcTexture = transientTextures[frameIndex][copyInfo.SrcTexture].Image;
        VkImage dstTexture = transientTextures[frameIndex][copyInfo.DstTexture].Image;

        VkFormat srcFormat = GetVkImageFormat(transientTextures[frameIndex][copyInfo.SrcTexture].TextureInfo.Data.Format);
        VkFormat dstFormat = GetVkImageFormat(transientTextures[frameIndex][copyInfo.DstTexture].TextureInfo.Data.Format);

        VkImageAspectFlags srcAspcetMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);
        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkImageCopy imageCopy
        {
            .srcSubresource
            {
                .aspectMask = srcAspcetMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .srcOffset {copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},

            .dstSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .dstOffset {copyInfo.DstOffset.x, copyInfo.DstOffset.y, copyInfo.DstOffset.z},

            .extent {
                static_cast<uint32_t>(copyInfo.Extent.x),
                static_cast<uint32_t>(copyInfo.Extent.y),
                static_cast<uint32_t>(copyInfo.Extent.z)
            }
        };

        vkCmdCopyImage(cmdBuffer, 
            srcTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);
    }

}

void RenderGraph::RecordTransientBufferToTextureCopy(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<BufferToTextureCopy>& copies = pass.transientBufferToTextureCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        BufferToTextureCopy copyInfo = copies[i];

        VkBuffer srcBuffer = transientBuffers[frameIndex][copyInfo.SrcBuffer].Buffer;
        VkImage dstImage = transientTextures[frameIndex][copyInfo.DstTexture].Image;

        VkFormat dstFormat = GetVkImageFormat(transientTextures[frameIndex][copyInfo.DstTexture].TextureInfo.Data.Format);

        VkImageAspectFlags dstAspectMask = GetVkImageAspectMaskBasedOnFormat(dstFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.SrcOffset,
            .bufferRowLength = copyInfo.BufferRowLenght,
            .bufferImageHeight = copyInfo.BufferHeightLenght,
            .imageSubresource
            {
                .aspectMask = dstAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
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
void RenderGraph::RecordTransientTextureToBufferCopy(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureToBufferCopy>& copies = pass.transientTextureToBufferCopies;

    for(uint32_t i = 0; i < copies.size(); i++)
    {
        TextureToBufferCopy copyInfo = copies[i];

        VkImage srcTexture = transientTextures[frameIndex][copyInfo.SrcTexture].Image;
        VkBuffer dstBuffer = transientBuffers[frameIndex][copyInfo.DstBuffer].Buffer;

        VkFormat srcFormat = GetVkImageFormat(transientTextures[frameIndex][copyInfo.SrcTexture].TextureInfo.Data.Format);

        VkImageAspectFlags srcAspectMask = GetVkImageAspectMaskBasedOnFormat(srcFormat);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = copyInfo.DstOffset,
            .bufferRowLength = copyInfo.BufferRowLenght,
            .bufferImageHeight = copyInfo.BufferHeightLenght,
            .imageSubresource
            {
                .aspectMask = srcAspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .imageOffset{copyInfo.SrcOffset.x, copyInfo.SrcOffset.y, copyInfo.SrcOffset.z},
            .imageExtent{
                static_cast<uint32_t>(copyInfo.Extent.x), 
                static_cast<uint32_t>(copyInfo.Extent.y), 
                static_cast<uint32_t>(copyInfo.Extent.z)}
        };

        vkCmdCopyImageToBuffer(cmdBuffer, srcTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
            dstBuffer, 1, &bufferImageCopy);
    }
}

void RenderGraph::RecordTransientBufferUpload(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
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

void RenderGraph::RecordTransientTextureUpload(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<TextureUpload>& uploads = pass.transientTextureUploads;
    // To complete
    for(uint32_t i = 0; i < uploads.size(); i++)
    {
        TextureUpload uploadInfo = uploads[i];

        BufferObject& srcBuffer = MemoryRegistry::GetBuffer(uploadInfo.SrcBufferId);
        TextureObject& dstTexture = MemoryRegistry::GetTexture(uploadInfo.DstTexture);

        VkFormat format = GetVkImageFormat(transientTextures[frameIndex][uploadInfo.DstTexture].TextureInfo.Data.Format);

        VkImageAspectFlags aspectMask = GetVkImageAspectMaskBasedOnFormat(format);

        VkBufferImageCopy bufferImageCopy
        {
            .bufferOffset = 0,
            .bufferRowLength = uploadInfo.BufferRowLenght,
            .bufferImageHeight = uploadInfo.BufferHeightLenght,
            .imageSubresource
            {
                .aspectMask = aspectMask,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
            .imageOffset {uploadInfo.DstOffset.x, uploadInfo.DstOffset.y, uploadInfo.DstOffset.z},
            .imageExtent {
                static_cast<uint32_t>(uploadInfo.Extent.x), 
                static_cast<uint32_t>(uploadInfo.Extent.y), 
                static_cast<uint32_t>(uploadInfo.Extent.z)}
        };

        vkCmdCopyBufferToImage(cmdBuffer, srcBuffer.Buffer, dstTexture.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &bufferImageCopy);

        MemoryRegistry::DestroyBuffer(uploadInfo.SrcBufferId);
    }
}

void RenderGraph::RecordDrawCalls(VkCommandBuffer& cmdBuffer, Pass& pass, uint32_t frameIndex)
{
    std::vector<std::pair<TransientTextureHandle, Usage>>& textures = pass.transientTextures;
    std::vector<std::pair<TransientTextureHandle, LoadStoreOp>>& loadStoreOps = pass.loadStoreOps;
    std::vector<DrawCall>& drawCalls = pass.drawCalls;

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

    std::vector<VkRenderingAttachmentInfo> colorsAttachmentInfo;
    VkRenderingAttachmentInfo depthAttachmentInfo{};
    VkRenderingAttachmentInfo stencilAttachmentInfo{};

    uint32_t width = UINT32_MAX;
    uint32_t height = UINT32_MAX;
    for(uint32_t i = 0; i < colorTargets.size(); i++)
    {
        TransientTextureHandle handle = colorTargets[i];

        uint32_t _width = transientTextures[frameIndex][handle.Id].TextureInfo.Data.Width;
        uint32_t _height = transientTextures[frameIndex][handle.Id].TextureInfo.Data.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);

        VkImageView imageView = transientTextures[frameIndex][handle.Id].ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != handle.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfo attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
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
        uint32_t _width = transientTextures[frameIndex][depthTarget.Id].TextureInfo.Data.Width;
        uint32_t _height = transientTextures[frameIndex][depthTarget.Id].TextureInfo.Data.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);


        VkImageView imageView = transientTextures[frameIndex][depthTarget.Id].ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != depthTarget.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfo attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue {.depthStencil{.depth{loadStoreOp.clearDepth}}}
        };

        depthAttachmentInfo = attachment;
    }

    if(useStencilTarget)
    {
        uint32_t _width = transientTextures[frameIndex][stencilTarget.Id].TextureInfo.Data.Width;
        uint32_t _height = transientTextures[frameIndex][stencilTarget.Id].TextureInfo.Data.Height;

        width = std::min(width, _width);
        height = std::min(height, _height);

        VkImageView imageView = transientTextures[frameIndex][stencilTarget.Id].ImageView;

        LoadStoreOp loadStoreOp;
        for(uint32_t j = 0; j < loadStoreOps.size(); j++)
        {
            if(loadStoreOps[j].first.Id != stencilTarget.Id) { continue; }

            loadStoreOp = loadStoreOps[j].second;
        }

        VkAttachmentLoadOp loadOp = GetVkLoadOp(loadStoreOp.loadOp);
        VkAttachmentStoreOp storeOp = GetVkStoreOp(loadStoreOp.storeOp);

        VkRenderingAttachmentInfo attachment
        {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp = loadOp,
            .storeOp = storeOp,
            .clearValue {.depthStencil{.stencil{loadStoreOp.clearStencil}}}
        };

        stencilAttachmentInfo = attachment;
    }

    // Invalid resolution
    if(width == UINT32_MAX || height == UINT32_MAX) { return; }

    VkRenderingInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
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

    vkCmdBeginRendering(cmdBuffer, &renderInfo);
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

            GraphicsShaderObject shader = ShaderRegistry::GetShaderObject(drawCall.ShaderHandle);

            CPUMesh& cpuMesh = MeshRegistry::GetMeshData(drawCall.MeshHandle);
            GraphicsMesh& graphicsMesh = MeshRegistry::GetGraphicsMesh(drawCall.MeshHandle);
            VkBuffer indexBuffer = MemoryRegistry::GetBuffer(graphicsMesh.IndexBuffer).Buffer;

            vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.Pipeline);

            VkDeviceSize offset {0};
            vkCmdBindIndexBuffer(cmdBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(cmdBuffer, graphicsMesh.IndexCount, drawCall.instanceCount, 0, 0, 0);
        }
    }
    vkCmdEndRendering(cmdBuffer);
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
    vkGetDeviceImageMemoryRequirements(GraphicsCore::Context.Device, &reqs, &memoryRequirements);

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
    vkGetDeviceBufferMemoryRequirements(GraphicsCore::Context.Device, &reqs, &memoryRequirements);

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

bool RenderGraph::ResizeTextureMemoryPoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
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

        vkDestroyImageView(GraphicsCore::Context.Device, texture.first.ImageView, nullptr);
        vkDestroyImage(GraphicsCore::Context.Device, texture.first.Image, nullptr);

        texturesPool.erase(texturesPool.begin() + i);
    }

    if(memoryBucket.used)
    {
        vmaFreeMemory(GraphicsCore::Context.Allocator, memoryBucket.Allocation);
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

    VkResult result = vmaAllocateMemory(GraphicsCore::Context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.used = true;

    return true;
}

bool RenderGraph::ResizeBufferMemoryPoolIfNeeded(const uint32_t bucketIndex, const uint64_t peakSize, const uint64_t peakAlignment)
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

        vkDestroyBuffer(GraphicsCore::Context.Device, buffer.first.Buffer, nullptr);

        buffersPool.erase(buffersPool.begin() + i);
    }

    if(memoryBucket.used)
    {
        vmaFreeMemory(GraphicsCore::Context.Allocator, memoryBucket.Allocation);
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

    VkResult result = vmaAllocateMemory(GraphicsCore::Context.Allocator, &memReqs, &allocInfo, 
        &memoryBucket.Allocation, &memoryBucket.AllocationInfo);

    if(result != VK_SUCCESS)
    {
        return false;
    }

    memoryBucket.used = true;

    return true;
}

void RenderGraph::UpdateTexturesPool(const uint32_t frameIndex)
{

    std::vector<uint32_t> freeTexturesSlot;
    for(uint32_t i = 0; i < texturesPool.size(); i++)
    {
        std::pair<TextureResource, bool>& data = texturesPool[i];

        if (data.second == true) 
        {
            data.first.FramesCount = 10; // TODO: Set with a valid frame delay
        }
        else 
        {
            if (data.first.FramesCount == 0) 
            { 
                freeTexturesSlot.push_back(i); 
            }
            else 
            {
                data.first.FramesCount--;
            }
        }
    }

    int32_t freeTexturesCount = freeTexturesSlot.size();
    int32_t freeTexturesIndex = 0;

    for(uint32_t i = 0; i < freeTexturesCount; i++)
    {
        uint32_t index = freeTexturesSlot[i];
        
        // Destroy old images
        TextureResource oldTexture = texturesPool[index].first;
        vkDestroyImageView(GraphicsCore::Context.Device, oldTexture.ImageView, nullptr);
        vkDestroyImage(GraphicsCore::Context.Device, oldTexture.Image, nullptr);
    }


    for(uint32_t i = 0; i < transientTextures[frameIndex].size(); i++)
    {
        TextureResource& data = transientTextures[frameIndex][i];

        MemoryRegistry::FreeTextureSlot(static_cast<TransientTextureHandle>(i));
        if(data.PooledImage) { continue; }

        data.FramesCount = 10; // TODO: Set with a valid frame delay

        if(freeTexturesIndex < freeTexturesCount)
        {
            uint32_t index = freeTexturesSlot[freeTexturesIndex];

            texturesPool[index] = std::pair{data, true};

            freeTexturesIndex++;
            
            continue;
        }

        texturesPool.push_back(std::pair{data, true});
    }

    // There are some textures which need to be erased
    for(int32_t i = freeTexturesCount - 1; i >= freeTexturesIndex; i--)
    {
        uint32_t index = freeTexturesSlot[i];

        texturesPool.erase(texturesPool.begin() + index);
    }

    for(uint32_t i = 0; i < texturesPool.size(); i++)
    {
        // All the textures can be used again
        texturesPool[i].second = false;
    }

    std::sort(texturesPool.begin(), texturesPool.end(), [](const std::pair<TextureResource, bool>& a, const std::pair<TextureResource, bool>& b) {
        return memcmp(&a.first.TextureInfo, &b.first.TextureInfo, sizeof(TextureInfo)) < 0;
    });
}

void RenderGraph::UpdateBuffersPool(const uint32_t frameIndex)
{

    std::vector<uint32_t> freeBuffersSlot;
    for(uint32_t i = 0; i < buffersPool.size(); i++)
    {
        std::pair<BufferResource, bool>& data = buffersPool[i];

        if (data.second == true) 
        {
            data.first.FramesCount = 10; // TODO: Set with a valid frame delay
        }
        else 
        {
            // Non è in uso, vediamo se è scaduta
            if (data.first.FramesCount == 0) 
            { 
                freeBuffersSlot.push_back(i); 
            }
            else 
            {
                data.first.FramesCount--;
            }
        }
    }

    int32_t freeBuffersCount = freeBuffersSlot.size();
    int32_t freeBuffersIndex = 0;

    for(uint32_t i = 0; i < freeBuffersCount; i++)
    {
        uint32_t index = freeBuffersSlot[i];

        // Destroy old buffer
        BufferResource oldBuffer = buffersPool[index].first;
        vkDestroyBuffer(GraphicsCore::Context.Device, oldBuffer.Buffer, nullptr);
    }

    for(uint32_t i = 0; i < transientBuffers[frameIndex].size(); i++)
    {
        BufferResource& data = transientBuffers[frameIndex][i];

        MemoryRegistry::FreeBufferSlot(static_cast<TransientBufferHandle>(i));

        if(data.PooledBuffer) { continue; }

        data.FramesCount = 10; // TODO: Set with a valid frame delay

        if(freeBuffersIndex < freeBuffersCount)
        {
            uint32_t index = freeBuffersSlot[freeBuffersIndex];

            buffersPool[index] = std::pair{data, true};

            freeBuffersIndex++;
            
            continue;
        }

        buffersPool.push_back(std::pair{data, true});
    }

    // There are some buffers which need to be erased
    for(int32_t i = freeBuffersCount - 1; i >= freeBuffersIndex; i--)
    {
        uint32_t index = freeBuffersSlot[i];

        buffersPool.erase(buffersPool.begin() + index);
    }

    for(uint32_t i = 0; i < buffersPool.size(); i++)
    {
        // All the buffers can be used again
        buffersPool[i].second = false;
    }

    std::sort(buffersPool.begin(), buffersPool.end(), [](const std::pair<BufferResource, bool>& a, const std::pair<BufferResource, bool>& b) {
        return memcmp(&a.first.BufferInfo, &b.first.BufferInfo, sizeof(BufferInfo)) < 0;
    });
}

TransientTextureHandle RenderGraph::RequestTransientTexture1D(TransientTextureInfo1D textureInfo)
{
    TransientTextureHandle handle = MemoryRegistry::ReserveTransientTextureSlot();

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

    transientRequestedTextures.push_back(data);

    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture2D(TransientTextureInfo2D textureInfo)
{
    TransientTextureHandle handle = MemoryRegistry::ReserveTransientTextureSlot();
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

    transientRequestedTextures.push_back(data);
    return handle;
}

TransientTextureHandle RenderGraph::RequestTransientTexture3D(TransientTextureInfo3D textureInfo)
{
    TransientTextureHandle handle = MemoryRegistry::ReserveTransientTextureSlot();
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

    transientRequestedTextures.push_back(data);
    return handle;
}

TransientBufferHandle RenderGraph::RequestTransientBuffer(TransientBufferInfo bufferInfo)
{
    TransientBufferHandle handle = MemoryRegistry::ReserveTransientBufferSlot();

    BufferInfo data
    {
        .Data.Size = bufferInfo.Size,
        .Data.Usage = static_cast<BufferUsage>(0)
    };

    transientRequestedBuffers.push_back(data);
    return handle;
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