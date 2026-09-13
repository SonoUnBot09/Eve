#pragma once

#include <vulkan/vulkan.hpp>
#include <imgui/imgui.h>

namespace Eve::Graphics
{

    class ImGUIBuilder
    {
        public:

            static bool Build();
            static void Destroy();

        private:

            static void SetGlobalStyle(ImGuiStyle& style);

            inline static VkDescriptorPool descriptorPool = nullptr;
    };
}