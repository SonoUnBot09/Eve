#include <eve/graphics/UI.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_vulkan.h>
#include <SDL3/SDL.h>

using namespace Eve::Graphics;

void UI::Window(std::string name, bool* open)
{
    if(isAWindowAlreadyOpen)
    {
        ImGui::End();
    }

    isAWindowAlreadyOpen = true;


    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking;

    ImGui::Begin(name.c_str(), open, flags);
}

void UI::DockableWindow(std::string name, bool* open)
{
    if(isAWindowAlreadyOpen)
    {
        ImGui::End();
    }

    isAWindowAlreadyOpen = true;

    ImGui::Begin(name.c_str(), open);
}

#pragma region Formats

bool UI::CollapsingHeader(std::string name)
{
    return ImGui::CollapsingHeader(name.c_str());
}

void UI::Separator()
{
    ImGui::Separator();
}

void UI::SeparatorText(std::string name)
{
    ImGui::SeparatorText(name.c_str());
}

void UI::Space()
{
    ImGui::NewLine();
}

#pragma endregion

#pragma region Float/Int

void UI::Float(std::string name, float& value, float step)
{
    ImGui::DragFloat(name.c_str(), &value, step);
}
void UI::Float(std::string name, float& value, float min, float max)
{
    ImGui::SliderFloat(name.c_str(), &value, min, max);
}

void UI::Integer(std::string name, int& value, int step)
{
    ImGui::DragInt(name.c_str(), &value, step);
}
void UI::Integer(std::string name, int& value, int min, int max)
{
    ImGui::SliderInt(name.c_str(), &value, min, max);
}

#pragma endregion

#pragma region Float2/Int2

void UI::Float2(std::string name, glm::vec2& value, float step)
{
    ImGui::DragFloat2(name.c_str(), (float*)&value, step);
}

void UI::Float2(std::string name, glm::vec2& value, float min, float max)
{
    ImGui::SliderFloat2(name.c_str(), (float*)&value, min, max);
}

void UI::Integer2(std::string name, glm::ivec2& value)
{
    ImGui::DragInt2(name.c_str(), (int*)&value);
}

void UI::Integer2(std::string name, glm::ivec2& value, int min, int max)
{
    ImGui::SliderInt2(name.c_str(), (int*)&value, min, max);
}

#pragma endregion

#pragma region Float3/Int3

void UI::Float3(std::string name, glm::vec3& value, float step)
{
    ImGui::DragFloat3(name.c_str(), (float*)&value, step);
}

void UI::Float3RGB(std::string name, glm::vec3& value, float step)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR;
    ImGui::ColorEdit3(name.c_str(), (float*)&value, flags);
}

void UI::Float3(std::string name, glm::vec3& value, float min, float max)
{
    ImGui::SliderFloat3(name.c_str(), (float*)&value, min, max);
}

void UI::Integer3(std::string name, glm::ivec3& value)
{
    ImGui::DragInt3(name.c_str(), (int*)&value);
}

void UI::Integer3(std::string name, glm::ivec3& value, int min, int max)
{
    ImGui::SliderInt3(name.c_str(), (int*)&value, min, max);
}

#pragma endregion

#pragma region Float4/Int4

void UI::Float4(std::string name, glm::vec4& value, float step)
{
    ImGui::DragFloat4(name.c_str(), (float*)&value, step);
}

void UI::Float4RGBA(std::string name, glm::vec4& value, float step)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR;
    ImGui::ColorEdit4(name.c_str(), (float*)&value, flags);
}

void UI::Float4(std::string name, glm::vec4& value, float min, float max)
{
    ImGui::SliderFloat4(name.c_str(), (float*)&value, min, max);
}

void UI::Integer4(std::string name, glm::ivec4& value)
{
    ImGui::DragInt4(name.c_str(), (int*)&value);
}

void UI::Integer4(std::string name, glm::ivec4& value, int min, int max)
{
    ImGui::SliderInt4(name.c_str(), (int*)&value, min, max);
}

#pragma endregion

#pragma region Color

void UI::ColorPicker3(std::string name, glm::vec3 &value)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;
    ImGui::ColorPicker3(name.c_str(), &value.x, flags);
}

void UI::ColorWheel3(std::string name, glm::vec3 &value)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_Float;
    ImGui::ColorPicker3(name.c_str(), &value.x, flags);
}

void UI::ColorPicker4(std::string name, glm::vec4 &value)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_Float;
    ImGui::ColorPicker4(name.c_str(), &value.x, flags);
}

void UI::ColorWheel4(std::string name, glm::vec4 &value)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_Float;
    ImGui::ColorPicker4(name.c_str(), &value.x, flags);
}

#pragma endregion

bool UI::Checkbox(std::string name, bool& value)
{
    return ImGui::Checkbox(name.c_str(), &value);
}

bool UI::Button(std::string name, glm::vec2 size)
{
    return ImGui::Button(name.c_str(), ImVec2(size.x, size.y));
}

void UI::SelectableList(const std::string& id, const std::vector<std::string>& items, uint32_t& currentIndex)
{
    uint32_t selectedIndex = currentIndex;

    const char* previewValue = (currentIndex < items.size()) ? items[currentIndex].c_str() : "";

    if (ImGui::BeginCombo(id.c_str(), previewValue))
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(items.size()); ++i)
        {
            const bool isSelected = (currentIndex == i);

            ImGui::PushID(i);

            if (ImGui::Selectable(items[i].c_str(), isSelected))
            {
                selectedIndex = i;
            }

            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    currentIndex = selectedIndex;
}

bool UI::IsMouseInteracting()
{
    ImGuiIO& io = ImGui::GetIO();

    return io.WantCaptureMouse;
}

bool UI::IsKeyboardInteracting()
{
    ImGuiIO& io = ImGui::GetIO();

    return io.WantCaptureKeyboard;
}

void UI::StartRecording()
{
    isAWindowAlreadyOpen = false;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspaceFlags);
}

void UI::EndRecording()
{
    if(isAWindowAlreadyOpen)
    {
        ImGui::End();
        isAWindowAlreadyOpen = false;
    }
}