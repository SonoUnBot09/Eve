#pragma once

#include <eve/math/Matrix4x4.hpp>
#include <eve/math/Vector2Int.hpp>
#include <eve/graphics/RenderViewHandle.hpp>
#include <eve/components/Transform.hpp>

using namespace Eve::Math;

namespace Eve::Graphics 
{
    struct RenderViewObject
    {
        Matrix4x4 ViewMatrix = Matrix4x4::Identity();
        Matrix4x4 InvViewMatrix = Matrix4x4::Identity();
        Matrix4x4 ProjectionMatrix = Matrix4x4::Identity();
        Matrix4x4 InvProjectionMatrix = Matrix4x4::Identity();
        Matrix4x4 VPMatrix = Matrix4x4::Identity();
        Matrix4x4 InvVPMatrix = Matrix4x4::Identity();
        Vector4 WorldPos;
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