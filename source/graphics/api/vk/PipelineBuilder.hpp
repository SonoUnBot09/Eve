#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    class PipelineBuilder
    {
        public:
            void SetupPushConstantData(uint32_t vertOffset, uint32_t vertStride, uint32_t fragOffset, uint32_t fragStride);
            void SetupShaders();
            void SetupDepthStencil(bool depthTest, bool depthWrite, bool stencilTest);

            void BuildGraphicsPipeline();
    };
}