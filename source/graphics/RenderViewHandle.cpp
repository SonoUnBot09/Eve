#include "registers/RenderViewRegistry.hpp"
#include <eve/graphics/RenderViewHandle.hpp>
#include <graphics/registers/RenderViewRegistry.hpp>

using namespace Eve::Graphics;

void RenderViewHandle::SetTRS(Transform& transform)
{
    RenderViewRegistry::SetTRS(*this, transform);
}

void RenderViewHandle::SetPerspective(float fovRadians, float aspect, float zNear, float zFar)
{
    RenderViewRegistry::SetPerspective(*this, fovRadians, aspect, zNear, zFar);
}

void RenderViewHandle::SetOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    RenderViewRegistry::SetOrtho(*this, left, right, bottom, top, zNear, zFar);
}