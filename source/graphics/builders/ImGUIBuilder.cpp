#include <graphics/GraphicsCore.hpp>
#include "ImGUIBuilder.hpp"
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui.h>
#include <eve/debug/Debug.hpp>
#include <graphics/ErrorManager.hpp>

using namespace Eve::Graphics;
using namespace Eve::Debug;

bool ImGUIBuilder::Build()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiStyle& style = ImGui::GetStyle();

    SetGlobalStyle(style);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDpiScaleViewports = true;
    io.ConfigDpiScaleFonts = true;

    VkDescriptorPoolSize pool_sizes[] = 
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 100 * static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;

    const Context context = GraphicsCore::Context;
    const Window window = GraphicsCore::Window;
    const Swapchain swapchain = GraphicsCore::Swapchain;

    VK_CHECK(vkCreateDescriptorPool(context.Device, &pool_info, nullptr, &descriptorPool));

    if (!ImGui_ImplSDL3_InitForVulkan(window.Window)) 
    {
        print("Unable to initialize ImGUI's SDL3 backend");
        return false;
    }

    VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo = {};
    pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    pipelineRenderingInfo.colorAttachmentCount = 1;
    pipelineRenderingInfo.pColorAttachmentFormats = &swapchain.Format;

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion = VK_API_VERSION_1_2;
    init_info.Instance = context.Instance;
    init_info.PhysicalDevice = context.PhysicalDevice;
    init_info.Device = context.Device;
    init_info.QueueFamily = context.GraphicsQueueIndex;
    init_info.Queue = context.GraphicsQueue;
    init_info.DescriptorPool = descriptorPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 3; 
    init_info.UseDynamicRendering = true;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo = pipelineRenderingInfo;

    ImGui_ImplVulkan_LoadFunctions(VK_API_VERSION_1_2, [](const char* function_name, void* user_data) 
    {
        return vkGetDeviceProcAddr(reinterpret_cast<VkDevice>(user_data), function_name);
    }, context.Device);

    if (!ImGui_ImplVulkan_Init(&init_info)) 
    {
        printError("Unable to inialize ImGUI's vulkan backend");
        return false;
    }

    return true;
}

void ImGUIBuilder::SetGlobalStyle(ImGuiStyle& style)
{
    ImVec4* colors = style.Colors;

    style.WindowRounding    = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 5.0f;
    style.ChildRounding     = 5.0f;

    style.WindowBorderSize  = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.ChildBorderSize   = 1.0f;

    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(8.0f, 5.0f);
    style.ItemSpacing       = ImVec2(8.0f, 6.0f);
   
    colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);

    colors[ImGuiCol_WindowBg]               = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    
    colors[ImGuiCol_PopupBg]                = ImVec4(0.12f, 0.12f, 0.13f, 0.98f);
    colors[ImGuiCol_Border]                 = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg]                = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

    colors[ImGuiCol_TitleBg]                = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.02f, 0.02f, 0.02f, 0.75f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);

    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);

    colors[ImGuiCol_CheckMark]              = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);
    
    colors[ImGuiCol_Button]                 = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);

    colors[ImGuiCol_Header]                 = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.22f, 0.22f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);

    colors[ImGuiCol_Separator]              = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.35f, 0.35f, 0.38f, 1.00f);

    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.14f, 0.14f, 0.16f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.24f, 0.24f, 0.26f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.35f, 0.35f, 0.38f, 0.95f);

    colors[ImGuiCol_Tab]                    = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.04f, 0.04f, 0.05f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);

    colors[ImGuiCol_DockingPreview]         = ImVec4(0.40f, 0.40f, 0.42f, 0.25f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
}

void ImGUIBuilder::Destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();

    ImGui::DestroyContext();

    if (descriptorPool != nullptr) 
    {
        vkDestroyDescriptorPool(GraphicsCore::Context.Device, descriptorPool, nullptr);
        descriptorPool = nullptr;
    }
}