#pragma once

#include "glm/geometric.hpp"
#include <glm/glm.hpp>


struct Camera
{
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;

    float speed;
    float sensitivity;

    float pitch;
    float yaw;

    Camera(glm::vec3 forward, glm::vec3 up, float speed, float sensitivity) : 
        forward(forward), up(up), right(glm::normalize(glm::cross(up, forward))),
        speed(speed), sensitivity(sensitivity), pitch(0), yaw(0)
    {}
};