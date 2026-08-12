#pragma once

#include "FrameData.hpp"

class FrameDataBuilder
{
    public:
        static bool Build(std::vector<FrameData>& frameData, VkSemaphore& timelineSemaphore);
};