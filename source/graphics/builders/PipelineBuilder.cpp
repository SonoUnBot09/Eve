#include "ContextBuilder.hpp"
#include "SlangCompiler.hpp"
#include <graphics/ResourceMapper.hpp>
#include "PipelineBuilder.hpp"
#include <graphics/ErrorManager.hpp>

#include <eve/Utils.hpp>
#include <eve/debug/Debug.hpp>

using namespace Eve::Graphics;
using namespace Eve::Debug;

void PipelineBuilder::Initialize()
{
    VkPushConstantRange pushConstantRange
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = 0,
        .size = 128
    };

    VkDescriptorSetLayout descriptorSetLayout = ResourceMapper::GetDescriptorSetLayout();
    VkPipelineLayoutCreateInfo pipelineLayoutCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange
    };

    VK_CHECK(vkCreatePipelineLayout(GraphicsCore::Context.Device, &pipelineLayoutCI, nullptr, &graphicsPipelineLayout));
}

bool PipelineBuilder::GetGraphicsPipelineLayout(VkPipelineLayout& graphicsPipelineLayout)
{
    if(PipelineBuilder::graphicsPipelineLayout)
    {
        graphicsPipelineLayout = PipelineBuilder::graphicsPipelineLayout;
        return true;
    }

    return false;
}

bool PipelineBuilder::BuildGraphicsPipeline(ShaderInfo shaderInfo, GraphicsShaderObject& shaderObject)
{
    ShaderBytecode shaders = SlangCompiler::CompileVertFrag(shaderInfo.ShaderModule.c_str());
    VkShaderModule vertexShader = CreateVertexModule(shaders);
    VkShaderModule fragmentShader = CreateFragmentModule(shaders);

    shaderObject.VertexModule = vertexShader;
    shaderObject.FragmentModule = fragmentShader;

    if(vertexShader == nullptr)
    {
        std::cerr << "Failed to create the vertex shader" << std::endl;
        return false;
    }
    if(fragmentShader == nullptr)
    {
        std::cerr << "Failed to create the fragment shader" << std::endl;
        return false;
    }
    
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo
    {
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader,
            .pName = "main"
        },
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader,
            .pName = "main"
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAsseblyInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = GetVkTopology(shaderInfo.Topology)
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = static_cast<VkBool32>(shaderInfo.DepthTest),
        .depthWriteEnable = static_cast<VkBool32>(shaderInfo.DepthWrite),
        .depthCompareOp = GetVkCompareOp(shaderInfo.CompareOp),
        .stencilTestEnable = static_cast<VkBool32>(shaderInfo.StencilTest)
    };

    VkPipelineViewportStateCreateInfo viewportInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterizationInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = GetVkPolygonMode(shaderInfo.PolygonMode),
        .cullMode = GetVkCullMode(shaderInfo.CullMode),
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = shaderInfo.LineWidth
    };

    VkPipelineMultisampleStateCreateInfo multisampleInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        //.rasterizationSamples = GetVkImageSamplesCount(shaderInfo.samplesCount)
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState colorAttachmentState
    {
       .blendEnable = VK_FALSE,
       .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentState
    };

    std::vector<VkDynamicState> dynamicStates
    {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    VkFormat colorFormat = GetVkImageFormat(shaderInfo.ColorFormat);
    VkPipelineRenderingCreateInfoKHR renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat = GetVkImageFormat(shaderInfo.DepthFormat)
    };

    VkGraphicsPipelineCreateInfo pipelineCI
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &renderInfo,
        .stageCount = static_cast<uint32_t>(shaderStagesInfo.size()),
        .pStages = shaderStagesInfo.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAsseblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multisampleInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicStateInfo,
        .layout = graphicsPipelineLayout
    };

    VK_CHECK(vkCreateGraphicsPipelines(GraphicsCore::Context.Device, nullptr, 1, &pipelineCI, nullptr,
        &shaderObject.Pipeline));

    return true;
}

VkShaderModule PipelineBuilder::CreateVertexModule(ShaderBytecode& input)
{
    const size_t size = input.vertex.size() * sizeof(uint32_t);
    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = input.vertex.data()
    };
    
    VkShaderModule shaderModule = nullptr;
    VK_CHECK(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule));

    return shaderModule;
}

VkShaderModule PipelineBuilder::CreateFragmentModule(ShaderBytecode& input)
{
    const size_t size = input.fragment.size() * sizeof(uint32_t);
    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = input.fragment.data()
    };
    
    VkShaderModule shaderModule = nullptr;
    VK_CHECK(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule));

    return shaderModule;
}

VkShaderModule PipelineBuilder::CreateComputeModule(ShaderBytecode& input)
{
    const size_t size = input.compute.size() * sizeof(uint32_t);
    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = input.compute.data()
    };
    
    VkShaderModule shaderModule = nullptr;
    VK_CHECK(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule));

    return shaderModule;
}