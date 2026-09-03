#pragma once

#include <glm/glm.hpp>
#include <eve/math/Vector3.hpp>
#include <eve/math/Quaternion.hpp>

struct Transform
{
    public:
        Eve::Math::Vector3 Position;
        Eve::Math::Quaternion Rotation;
        Eve::Math::Vector3 Scale;
};