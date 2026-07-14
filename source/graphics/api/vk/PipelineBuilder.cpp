#include <graphics/api/vk/ContextBuilder.hpp>
#include <graphics/api/vk/PipelineBuilder.hpp>
#include <graphics/api/vk/ResourceMapper.hpp>

#include <Eve/Utils.hpp>
#include <Eve/Debug.hpp>

using namespace Eve::Graphics;
using namespace Debug;

bool PipelineBuilder::BuildGraphicsPipeline(PipelineInfo pipelineInfo, PipelineHandle& handle)
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

    VkPipelineLayout layout;
    if(vkCreatePipelineLayout(ContextBuilder::context.Device, &pipelineLayoutCI, nullptr, &layout) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline layout");
        return false;
    }

    VkShaderModule vertexShader = CreateShaderModule(pipelineInfo.VertShaderPath, shaderc_shader_kind::shaderc_glsl_vertex_shader);
    VkShaderModule fragmentShader = CreateShaderModule(pipelineInfo.FragShaderPath, shaderc_shader_kind::shaderc_glsl_fragment_shader);

    if(vertexShader == nullptr)
    {
        return false;
    }

    if(fragmentShader == nullptr)
    {
        return false;
    }
    
    const char* entryPoint = "main";
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo
    {
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertexShader,
            .pName = entryPoint
        },
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragmentShader,
            .pName = entryPoint
        }
    };

    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    uint32_t offset = 0;
    for(uint32_t i = 0; i < pipelineInfo.VerticesAttributes.size(); i++)
    {
        VkFormat format = GetVkImageFormat(pipelineInfo.VerticesAttributes[i]);
        uint32_t stride = GetVkImageFormatSize(pipelineInfo.VerticesAttributes[i]);
        VkVertexInputAttributeDescription attribute
        {
            .location = i,
            .binding = 0,
            .format = format,
            .offset = offset
        };

        vertexAttributes.push_back(attribute);

        offset += stride;
    }

    VkVertexInputBindingDescription vertexBinding
    {
        .binding = 0,
        .stride = offset,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBinding,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributes.size()),
        .pVertexAttributeDescriptions = vertexAttributes.data(),
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
        .layout = layout
    };

    VkPipeline pipeline;
    if(vkCreateGraphicsPipelines(ContextBuilder::context.Device, nullptr, 1, &pipelineCI, nullptr, &pipeline) != VK_SUCCESS)
    {
        printError("Unable to create the graphics pipeline");
        return false;
    }

    Pipeline graphicsPipeline
    {
        .Layout = layout,
        .Pipeline = pipeline
    };

    handle.Id = pipelines.size();

    pipelines.push_back(graphicsPipeline);

    return true;
}

VkShaderModule PipelineBuilder::CreateShaderModule(std::string path, shaderc_shader_kind kind)
{
    std::string source = Utils::readTextFile(path);
    if(source.empty())
    {
        printError("Unable to find the shader at path '" + path + "'");
        return nullptr;
    }

    shaderc::Compiler compiler;
    shaderc::CompileOptions compOpts;
    compOpts.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    compOpts.SetTargetSpirv(shaderc_spirv_version_1_6);
    compOpts.SetSourceLanguage(shaderc_source_language_glsl);
    compOpts.SetOptimizationLevel(shaderc_optimization_level_performance);

    std::string shaderName = "shader";
    if(kind == shaderc_shader_kind::shaderc_glsl_vertex_shader)
    {
        shaderName = "vertex shader";
    }
    else 
    {
        shaderName = "fragment shader";
    }

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv
    (
        source, 
        kind, 
        shaderName.c_str(),
        compOpts
    );

    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        printError("Unable to compile the shader at path '" + path + "'");
        return nullptr;
    }

    const size_t size = (result.cend() - result.cbegin()) * sizeof(uint32_t);
    VkShaderModuleCreateInfo shaderModuleCI
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = result.cbegin()
    };
    
    VkShaderModule shaderModule = nullptr;
    if(vkCreateShaderModule(ContextBuilder::context.Device, &shaderModuleCI, nullptr, &shaderModule) != VK_SUCCESS)
    {
        printError("Unable to create the shader module of the shader at path '" + path + "'");
        return nullptr;
    }

    return shaderModule;
}