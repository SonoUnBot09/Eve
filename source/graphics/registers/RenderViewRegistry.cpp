#include "RenderViewRegistry.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/matrix.hpp"

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

    glm::mat4 worldMatrix = 
        glm::translate(glm::mat4(1.0f), transform.Position) *
        glm::mat4_cast(transform.Rotation) * 
        glm::scale(glm::mat4(1.0f), transform.Scale);

    glm::mat4 viewMatrix = glm::inverse(worldMatrix);

    renderView.ViewMatrix = viewMatrix;
    renderView.InvViewMatrix = worldMatrix;
    renderView.WorldPos = glm::vec4(transform.Position, 1.0f);

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = glm::inverse(renderView.VPMatrix);
}
void RenderViewRegistry::SetPerspective(RenderViewHandle handle, float fovRadians, float aspect, float zNear, float zFar)
{
    RenderViewObject& renderView = renderViews[handle.Id];

    glm::mat4 projectionMatrix = glm::perspective(fovRadians, aspect, zNear, zFar);
    glm::mat4 invProjectionMatrix = glm::inverse(projectionMatrix);

    renderView.ProjectionMatrix = projectionMatrix;
    renderView.InvProjectionMatrix = invProjectionMatrix;

    renderView.NearPlane = zNear;
    renderView.FarPlane = zFar;

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = glm::inverse(renderView.VPMatrix);
}
void RenderViewRegistry::SetOrtho(RenderViewHandle handle, float left, float right, float bottom, float top, float zNear, float zFar)
{
    RenderViewObject& renderView = renderViews[handle.Id];

    glm::mat4 projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
    glm::mat4 invProjectionMatrix = glm::inverse(projectionMatrix);

    renderView.ProjectionMatrix = projectionMatrix;
    renderView.InvProjectionMatrix = invProjectionMatrix;

    renderView.NearPlane = zNear;
    renderView.FarPlane = zFar;

    renderView.VPMatrix = renderView.ProjectionMatrix * renderView.ViewMatrix;
    renderView.InvVPMatrix = glm::inverse(renderView.VPMatrix);
}