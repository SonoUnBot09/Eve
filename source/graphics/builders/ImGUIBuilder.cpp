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

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

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