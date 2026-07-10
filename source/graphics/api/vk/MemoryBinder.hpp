#pragma once

#include <cstdint>
#include <vector>

#include <graphics/api/vk/ContextBuilder.hpp>

namespace Eve::Graphics
{
    class MemoryBinder
    {
        public:
            void CreateGlobalDescriptor(uint32_t imagesCount, uint32_t samplersCount, uint32_t buffersCount);        
            
        private:

            VkDescriptorSetLayout layout;
            VkDescriptorPool pool;
            std::vector<VkDescriptorSet> sets {3};
    };
}