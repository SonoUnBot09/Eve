#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform
{
    public:
        glm::vec3 Position;
        glm::quat Rotation;
        glm::vec3 Scale;
};