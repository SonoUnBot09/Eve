#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Eve 
{
    class Core;
}

namespace Eve::Graphics
{
    class UI
    {
        public:

            static void Window(std::string name, bool* open = nullptr);
            static void DockableWindow(std::string name, bool* open = nullptr);

            static bool CollapsingHeader(std::string name);
            static void Separator();
            static void SeparatorText(std::string name);
            static void Space();

            static void Float(std::string name, float& value, float step = 1.0f);
            static void Float(std::string name, float& value, float min, float max);

            static void Float2(std::string name, glm::vec2& value, float step = 1.0f);
            static void Float2(std::string name, glm::vec2& value, float min, float max);

            static void Float3(std::string name, glm::vec3& value, float step = 1.0f);
            static void Float3RGB(std::string name, glm::vec3& value, float step = 1.0f);
            static void Float3(std::string name, glm::vec3& value, float min, float max);

            static void Float4(std::string name, glm::vec4& value, float step = 1.0f);
            static void Float4RGBA(std::string name, glm::vec4& value, float step = 1.0f);
            static void Float4(std::string name, glm::vec4& value, float min, float max);

            static void Integer(std::string name, int& value, int step = 1);
            static void Integer(std::string name, int& value, int min, int max);

            static void Integer2(std::string name, glm::ivec2& value);
            static void Integer2(std::string name, glm::ivec2& value, int min, int max);

            static void Integer3(std::string name, glm::ivec3& value);
            static void Integer3(std::string name, glm::ivec3& value, int min, int max);

            static void Integer4(std::string name, glm::ivec4& value);
            static void Integer4(std::string name, glm::ivec4& value, int min, int max);

            static void ColorPicker3(std::string name, glm::vec3& value);
            static void ColorWheel3(std::string name, glm::vec3& value);
            static void ColorPicker4(std::string name, glm::vec4& value);
            static void ColorWheel4(std::string name, glm::vec4& value);

            static bool Checkbox(std::string name, bool& value);
            static bool Button(std::string name, glm::vec2 = glm::vec2(100.0f, 100.0f));
            static void SelectableList(const std::string& name, const std::vector<std::string>& items, uint32_t& currentIndex);

            static bool IsMouseInteracting();
            static bool IsKeyboardInteracting();

        private:

            static void StartRecording();
            static void EndRecording();

            static inline bool isMouseInteracting = false;          // is mouse interacting with the ui?
            static inline bool isKeyboardInteracting = false;       // is the keyboard interacting with the ui?

            static inline bool isAWindowAlreadyOpen = false;

            friend class ::Eve::Core;
    };
}