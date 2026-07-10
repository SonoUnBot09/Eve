#pragma once

#include <cstdint>

#include <graphics/api/vk/ContextBuilder.hpp>

class MemoryBinder
{
    public:
        void CreateGlobalDescriptor(uint32_t imagesCount, uint32_t samplersCount, uint32_t buffersCount);        
        
    private:

        VkDescriptorSetLayout layout;
        VkDescriptorSet set;
        VkDescriptorPool pool;
};