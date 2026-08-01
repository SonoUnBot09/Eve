#pragma once

#include <vulkan/vulkan.hpp>
#include <graphics/Resources.hpp>
#include <graphics/Resources.hpp>

#include <vector>

namespace Eve::Graphics
{
    class CommandManager
    {
        public:
            static CommandPoolHandle AllocateCommandPool();
            static CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle pool);

        private:
            inline static std::vector<VkCommandPool> cmdPools;
            inline static std::vector<VkCommandBuffer> cmdBuffers;

            inline static std::vector<uint32_t> cmdPoolFreeSlots;
            inline static std::vector<uint32_t> cmdBufferFreeSlots;
    };
}