#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>

namespace Eve::Window
{
    class Window
    {
        public:
            static void SetFullscreen(bool isFullscreen);
            static void SetBordered(bool isBordered);
            static void SetResizable(bool isResizable);
            static void SetResolution(uint32_t width, uint32_t height);
            static void SetPosition(uint32_t x, uint32_t y);
            static void SetPositionAtCenter();
            static void SetWindowTitle(std::string title);
            static glm::ivec2 GetWindowSize();
            static bool SetIcon(std::string path);
    };
}