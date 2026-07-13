#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <mutex>

#include <graphics/api/vk/ContextBuilder.hpp>
#include <graphics/api/vk/MemoryManager.hpp>
#include <EveSettings.hpp>

#include <Eve/graphics/Image.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    class ResourceMapper
    {
        public:
            static void CreateGlobalDescriptor(uint32_t maxImagesCount, uint32_t maxSamplersCount, uint32_t maxBuffersCount);        
            
            static void ScheduleImageMapping(ImageHandle handle);
            static void ScheduleSamplerMapping(SamplerHandle handle);
            static void ScheduleBufferMapping(BufferHandle handle);

            static bool MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex);
        private:

            inline static VkDescriptorSetLayout layout;
            inline static VkDescriptorPool pool;
            inline static std::vector<VkDescriptorSet> sets {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            inline static std::vector<BufferHandle> BDABuffers {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            
            inline static std::vector<VkWriteDescriptorSet> descriptorSetWrites;
            inline static std::vector<VkDeviceAddress> buffersAddress;
            inline static std::vector<VkBufferCopy> copyRegions;

            inline static std::vector<BufferHandle> stagingBufferHandles {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            inline static std::vector<Buffer> stagingBuffers{Eve::Settings::MAX_FRAMES_IN_FLIGHT};

            inline static std::mutex imagesMutex;
            inline static std::mutex samplersMutex;
            inline static std::mutex buffersMutex;
            inline static std::vector<std::pair<ImageHandle, uint32_t>> imagesToMap;
            inline static std::vector<std::pair<SamplerHandle, uint32_t>> samplersToMap;
            inline static std::vector<std::pair<BufferHandle, uint32_t>> buffersToMap;

            inline static std::vector<uint32_t> imageToMapFreeSlots;
            inline static std::vector<uint32_t> samplerToMapFreeSlots;
            inline static std::vector<uint32_t> bufferToMapFreeSlots;
    };
}