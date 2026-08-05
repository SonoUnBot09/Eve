#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <mutex>

#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include <EveSettings.hpp>

#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Sampler.hpp>
#include <Eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    class ResourceMapper
    {
        public:
        
            static void CreateGlobalDescriptor(uint32_t maxImagesCount, uint32_t maxSamplersCount, uint32_t maxBuffersCount);
            static void DestroyGlobalDescriptor();        
            
            static void ScheduleImageMapping(TextureHandle handle, TextureInfo& textureInfo);
            static void ScheduleImageMapping(TransientTextureHandle handle, TextureInfo& textureInfo);
            static void ScheduleSamplerMapping(SamplerHandle handle);
            static void ScheduleBufferMapping(BufferHandle handle);
            static void ScheduleBufferMapping(TransientBufferHandle handle);

            static bool MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex);

            static inline VkDescriptorSetLayout GetDescriptorSetLayout() { return layout; }
        private:

            static std::vector<std::pair<TextureHandle, uint32_t>>& GetSampledVector(TextureType textureType);
            static std::vector<std::pair<TextureHandle, uint32_t>>& GetStorageVector(TextureType textureType);
            static std::vector<uint32_t>& GetFreeSampledSlotsVector(TextureType textureType);
            static std::vector<uint32_t>& GetFreeStorageSlotsVector(TextureType textureType);


            inline static VkDescriptorSetLayout layout;
            inline static VkDescriptorPool pool;
            inline static std::vector<VkDescriptorSet> sets {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            inline static std::vector<BufferHandle> BDABuffers {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            
            inline static std::vector<VkWriteDescriptorSet> descriptorSetWrites;
            inline static std::vector<VkDeviceAddress> buffersAddress;
            inline static std::vector<VkBufferCopy> copyRegions;

            inline static std::vector<BufferHandle> stagingBufferHandles {Eve::Settings::MAX_FRAMES_IN_FLIGHT};
            inline static std::vector<BufferObject> stagingBuffers{Eve::Settings::MAX_FRAMES_IN_FLIGHT};

            inline static std::mutex imagesMutex;
            inline static std::mutex samplersMutex;
            inline static std::mutex buffersMutex;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesSampled1DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesStorage1DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesSampled2DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesStorage2DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesSampled3DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesStorage3DToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesSampledCubeToMap;
            inline static std::vector<std::pair<TextureHandle, uint32_t>> imagesStorageCubeToMap;
            inline static std::vector<std::pair<SamplerHandle, uint32_t>> samplersToMap;
            inline static std::vector<std::pair<BufferHandle, uint32_t>> buffersToMap;

            inline static std::vector<uint32_t> imageSampled1DToMapFreeSlots;
            inline static std::vector<uint32_t> imageStorage1DToMapFreeSlots;
            inline static std::vector<uint32_t> imageSampled2DToMapFreeSlots;
            inline static std::vector<uint32_t> imageStorage2DToMapFreeSlots;
            inline static std::vector<uint32_t> imageSampled3DToMapFreeSlots;
            inline static std::vector<uint32_t> imageStorage3DToMapFreeSlots;
            inline static std::vector<uint32_t> imageSampledCubeToMapFreeSlots;
            inline static std::vector<uint32_t> imageStorageCubeToMapFreeSlots;
            inline static std::vector<uint32_t> samplerToMapFreeSlots;
            inline static std::vector<uint32_t> bufferToMapFreeSlots;
    };
}