#include "RenderViewRegistry.hpp"
#include "eve/math/Matrix4x4.hpp"

using namespace Eve::Graphics;

RenderViewHandle RenderViewRegistry::CreateRenderView()
{
    RenderViewHandle handle;
    if(freeSlots.empty())
    {
        handle.Id = renderViews.size();
        handle.GenerationId = 0;

        renderViews.emplace_back(RenderViewObject{});
        generationIds.push_back(0);
    }
    else 
    {
        uint32_t index = freeSlots.back();
        freeSlots.pop_back();

        renderViews[index] = RenderViewObject{};
        generationIds[index]++;

        handle.Id = index;
        handle.GenerationId = generationIds[index];
    }

    return handle;
}
void RenderViewRegistry::DestroyRenderView(RenderViewHandle handle)
{
    freeSlots.push_back(handle.Id);
}
void RenderViewRegistry::SetTRS(RenderViewHandle handle, Transform& transform)
{
    RenderViewObject& renderView = renderViews[handle.Id];

    Matrix4x4 viewMatrix = Matrix4x4::TRS(transform.Position, transform.Rotation, transform.Scale);
    Matrix4x4 invViewMatrix = viewMatrix.Inverse();

    renderView.ViewMatrix = viewMatrix;
    renderView.InvViewMatrix = invViewMatrix;
    renderView.WorldPos = Vector4(transform.Position.x, transform.Position.y, transform.Position.z, 1.0f);

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = renderView.VPMatrix.Inverse();
}
void RenderViewRegistry::SetPerspective(RenderViewHandle handle, float fovRadians, float aspect, float zNear, float zFar)
{
    RenderViewObject& renderView = renderViews[handle.Id];

    Matrix4x4 projectionMatrix = Matrix4x4::Perspective(fovRadians, aspect, zNear, zFar);
    Matrix4x4 invProjectionMatrix = projectionMatrix.Inverse();

    renderView.ProjectionMatrix = projectionMatrix;
    renderView.InvViewMatrix = invProjectionMatrix;

    renderView.NearPlane = zNear;
    renderView.FarPlane = zFar;

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = renderView.VPMatrix.Inverse();
}
void RenderViewRegistry::SetOrtho(RenderViewHandle handle, float left, float right, float bottom, float top, float zNear, float zFar)
{
    RenderViewObject& renderView = renderViews[handle.Id];

    Matrix4x4 projectionMatrix = Matrix4x4::Ortho(left, right, bottom, top, zNear, zFar);
    Matrix4x4 invProjectionMatrix = projectionMatrix.Inverse();

    renderView.ProjectionMatrix = projectionMatrix;
    renderView.InvViewMatrix = invProjectionMatrix;

    renderView.NearPlane = zNear;
    renderView.FarPlane = zFar;

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = renderView.VPMatrix.Inverse();
}