#pragma once

#include <eve/graphics/RenderViewHandle.hpp>
#include <eve/components/Transform.hpp>

namespace Eve::Graphics 
{
    struct RenderViewObject
    {
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        glm::mat4 InvViewMatrix = glm::mat4(1.0f);
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 InvProjectionMatrix =glm::mat4(1.0f);
        glm::mat4 VPMatrix = glm::mat4(1.0f);
        glm::mat4 InvVPMatrix =glm::mat4(1.0f);
        glm::vec4 WorldPos;
        float NearPlane;
        float FarPlane;
    };

    class RenderViewRegistry
    {
        public:

            static RenderViewHandle CreateRenderView();
            static void DestroyRenderView(RenderViewHandle handle);

            static void SetTRS(RenderViewHandle handle, Transform& transform);
            static void SetPerspective(RenderViewHandle handle, float fovRadians, float aspect, float zNear, float zFar);
            static void SetOrtho(RenderViewHandle handle, float left, float right, float bottom, float top, float zNear, float zFar);

            inline static uint32_t GetRenderViewsCount() { return renderViews.size(); }
            inline static void* GetRenderViewsPtr() { return renderViews.data(); }

        private:
            inline static std::vector<RenderViewObject> renderViews;    inline static std::vector<uint32_t> generationIds;
            inline static std::vector<uint32_t> freeSlots;
            
    };
}