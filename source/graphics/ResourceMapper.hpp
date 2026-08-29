#pragma once

#include <cstdint>
#include <vector>
#include <mutex>

#include <graphics/builders/ContextBuilder.hpp>
#include <graphics/registers/MemoryRegistry.hpp>
#include "Resources.hpp"
#include <EveSettings.hpp>

#include <eve/graphics/Texture.hpp>
#include <eve/graphics/Sampler.hpp>
#include <eve/graphics/Buffer.hpp>

namespace Eve::Graphics
{
    class ResourceMapper
    {
        public:
        
            static void CreateGlobalDescriptor(uint32_t maxImagesCount, uint32_t maxSamplersCount, uint32_t maxBuffersCount);
            static void DestroyGlobalDescriptor();        
            
            static void ScheduleImageMapping(TextureHandle handle, VkImageView imageView, TextureInfo& textureInfo);
            static void ScheduleImageMapping(TransientTextureHandle handle, VkImageView imageView, TextureInfo& textureInfo);
            static void ScheduleSamplerMapping(SamplerHandle handle, VkSampler sampler);
            static void ScheduleBufferMapping(BufferHandle handle, VkBuffer buffer);
            static void ScheduleBufferMapping(TransientBufferHandle handle, VkBuffer buffer);

            static void MapResources(VkCommandBuffer cmdBuffer, uint32_t frameIndex);

            static inline VkDescriptorSetLayout GetDescriptorSetLayout() { return layout; }
            static inline VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) { return sets[frameIndex]; }
        private:

            struct TextureToMap
            {
                VkImageView ImageView;
                uint32_t Id;
                uint32_t Countdown;
            };

            struct SamplerToMap
            {
                VkSampler Sampler;
                uint32_t Id;
                uint32_t Countdown;
            };

            struct BufferToMap
            {
                VkBuffer Buffer;
                uint32_t Id;
                uint32_t Countdown;
            };

            static std::vector<TextureToMap>& GetSampledVector(TextureType textureType);
            static std::vector<TextureToMap>& GetStorageVector(TextureType textureType);
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
            inline static std::vector<BufferObject> stagingBuffers {Eve::Settings::MAX_FRAMES_IN_FLIGHT};

            inline static std::mutex imagesMutex;
            inline static std::mutex samplersMutex;
            inline static std::mutex buffersMutex;
            inline static std::vector<TextureToMap> imagesSampled1DToMap;
            inline static std::vector<TextureToMap> imagesStorage1DToMap;
            inline static std::vector<TextureToMap> imagesSampled2DToMap;
            inline static std::vector<TextureToMap> imagesStorage2DToMap;
            inline static std::vector<TextureToMap> imagesSampled3DToMap;
            inline static std::vector<TextureToMap> imagesStorage3DToMap;
            inline static std::vector<TextureToMap> imagesSampledCubeToMap;
            inline static std::vector<TextureToMap> imagesStorageCubeToMap;
            inline static std::vector<SamplerToMap> samplersToMap;
            inline static std::vector<BufferToMap> buffersToMap;

            inline static std::vector<uint32_t> imagesSampled1DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesStorage1DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesSampled2DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesStorage2DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesSampled3DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesStorage3DToMapFreeSlots;
            inline static std::vector<uint32_t> imagesSampledCubeToMapFreeSlots;
            inline static std::vector<uint32_t> imagesStorageCubeToMapFreeSlots;
            inline static std::vector<uint32_t> samplersToMapFreeSlots;
            inline static std::vector<uint32_t> buffersToMapFreeSlots;
    };
}