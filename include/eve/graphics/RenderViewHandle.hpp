#pragma once

#include <cstdint>
#include <eve/components/Transform.hpp>

namespace Eve::Graphics 
{
    struct RenderViewHandle
    {
        uint32_t Id;
        uint32_t GenerationId;

        void SetTRS(Transform& transform);
        void SetPerspective(float fovRadians, float aspect, float zNear, float zFar);
        void SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
    };
}