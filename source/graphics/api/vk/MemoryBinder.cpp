#include "graphics/api/vk/ContextBuilder.hpp"
#include <graphics/api/vk/MemoryBinder.hpp>
#include <graphics/api/vk/VulkanMapping.hpp>
#include <EveSettings.hpp>
#include <vector>

using namespace Eve::Graphics;

void MemoryBinder::CreateGlobalDescriptor(uint32_t imagesCount, uint32_t samplersCount, uint32_t buffersCount)
{
    #pragma region Layout

        std::vector<VkDescriptorSetLayoutBinding> bindingsLayout;
        VkDescriptorSetLayoutBinding imagesDescriptor
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = imagesCount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        VkDescriptorSetLayoutBinding samplersDescriptor
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = samplersCount,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
        };

        VkDescriptorSetLayoutBinding buffersDescriptor
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = buffersCount,
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

        vkCreateDescriptorSetLayout(ContextBuilder::context.Device, &layoutCI, nullptr, &layout);

    #pragma endregion

    #pragma region Pool

        std::vector<VkDescriptorPoolSize> poolSize;

        VkDescriptorPoolSize imageDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = imagesCount
        };

        VkDescriptorPoolSize samplerDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = samplersCount
        };

        VkDescriptorPoolSize bufferDescriptorSize
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = buffersCount
        };

        poolSize.push_back(imageDescriptorSize);
        poolSize.push_back(samplerDescriptorSize);
        poolSize.push_back(bufferDescriptorSize);

        VkDescriptorPoolCreateInfo poolCI
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
            .pPoolSizes = poolSize.data()
        };

        vkCreateDescriptorPool(ContextBuilder::context.Device, &poolCI, nullptr, &pool);
    
    #pragma endregion

    #pragma region Set

        VkDescriptorSetAllocateInfo setAllocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = pool,
            .descriptorSetCount = Eve::Settings::MAX_FRAMES_IN_FLIGHT,
            .pSetLayouts = &layout
        };

        vkAllocateDescriptorSets(ContextBuilder::context.Device, &setAllocInfo, sets.data());


        //vmaCreateBuffer(ContextBuilder::context.Allocator, const VkBufferCreateInfo * _Nonnull pBufferCreateInfo, const VmaAllocationCreateInfo * _Nonnull pAllocationCreateInfo, VkBuffer  _Nullable * _Nonnull pBuffer, VmaAllocation  _Nullable * _Nonnull pAllocation, VmaAllocationInfo * _Nullable pAllocationInfo)
    #pragma endregion
}