#pragma once

#include <vulkan/vulkan.hpp>

namespace Eve::Graphics
{
    class ImGUIBuilder
    {
        public:
            static bool Build();
            static void Destroy();

        private:
            inline static VkDescriptorPool descriptorPool = nullptr;
    };
}