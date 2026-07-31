#include "ContextBuilder.hpp"
#include "SlangCompiler.hpp"
#include <graphics/ResourceMapper.hpp>
#include "PipelineBuilder.hpp"

#include <Eve/Utils.hpp>
#include <Eve/Debug.hpp>

using namespace Eve::Graphics;
using namespace Debug;

bool PipelineBuilder::BuildGraphicsPipeline(ShaderInfo pipelineInfo, GraphicsShaderObject& shaderObject)
{
    std::vector<VkPushConstantRange> pushConstantRanges;

    if((pipelineInfo.VertOffset + pipelineInfo.VertStride) > 128 || (pipelineInfo.FragOffset + pipelineInfo.FragStride) > 128)
    {
        printError("Unable to create the graphics pipeline. Push Constant data cannot be more than 128 bytes");
        return false;
    }

    if(pipelineInfo.VertStride > 0)
    {
        VkPushConstantRange pushConstant
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = pipelineInfo.VertOffset,
            .size = pipelineInfo.VertStride
        };

        pushConstantRanges.push_back(pushConstant);
    }

    if(pipelineInfo.FragStride > 0)
    {
        VkPushConstantRange pushConstant
        {
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = pipelineInfo.FragOffset,
            .size = pipelineInfo.FragStride
        };

        pushConstantRanges.push_back(pushConstant);
    }

    VkDescriptorSetLayout descriptorSetLayout = ResourceMapper::GetDescriptorSetLayout();
    VkPipelineLayoutCreateInfo pipelineLayoutCI
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
        .pPushConstantRanges = pushConstantRanges.data()
    };

    if(vkCreatePipelineLayout(GraphicsCore::Context.Device, &pipelineLayoutCI, nullptr, &shaderObject.Layout) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline layout");
        return false;
    }

    ShaderBytecode shaders = SlangCompiler::CompileVertFrag(pipelineInfo.ShaderPath.c_str());
    VkShaderModule vertexShader = CreateVertexModule(shaders);
    VkShaderModule fragmentShader = CreateFragmentModule(shaders);

    shaderObject.VertexModule = vertexShader;
    shaderObject.FragmentModule = fragmentShader;

    if(vertexShader == nullptr)
    {
        return false;
    }
    if(fragmentShader == nullptr)
    {
        return false;
    }
    
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo
    {
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader,
            .pName = "vertex"
        },
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader,
            .pName = "fragment"
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
        .topology = GetVkTopology(pipelineInfo.Topology)
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = static_cast<VkBool32>(pipelineInfo.DepthTest),
        .depthWriteEnable = static_cast<VkBool32>(pipelineInfo.DepthWrite),
        .depthCompareOp = GetVkCompareOp(pipelineInfo.CompareOp),
        .stencilTestEnable = static_cast<VkBool32>(pipelineInfo.StencilTest)
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
        .polygonMode = GetVkPolygonMode(pipelineInfo.PolygonMode),
        .cullMode = GetVkCullMode(pipelineInfo.CullMode),
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = pipelineInfo.LineWidth
    };

    VkPipelineMultisampleStateCreateInfo multisampleInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = GetVkImageSamplesCount(pipelineInfo.samplesCount)
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

    VkFormat colorFormat = GetVkImageFormat(pipelineInfo.ColorFormat);
    VkPipelineRenderingCreateInfo renderInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .colorAttachmentCount = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat = GetVkImageFormat(pipelineInfo.DepthFormat)
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
        .layout = shaderObject.Layout
    };

    if(vkCreateGraphicsPipelines(GraphicsCore::Context.Device, nullptr, 1, &pipelineCI, nullptr, &shaderObject.Pipeline) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline");
        return false;
    }

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
    if(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Unable to create the shader module");
        return nullptr;
    }

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
    if(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Unable to create the shader module");
        return nullptr;
    }

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
    if(vkCreateShaderModule(GraphicsCore::Context.Device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Unable to create the shader module");
        return nullptr;
    }

    return shaderModule;
}