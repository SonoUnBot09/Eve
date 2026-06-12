#version 460
/*
const vec3 positions[] = vec3[]
(
    vec3(0.0, -0.5, 0.0), // Top
    vec3(-0.5, 0.5, 0.0), // Bottom left
    vec3(0.5, 0.5, 0.0)   // Bottom right
);
vec3(-0.5, -0.5 , 1.0),
vec3(0.5, -0.5, 1.0),
vec3(-0.5,  0.5, 1.0),
*/
const vec3 positions[] = vec3[]
(
    vec3(-0.5, -0.5 , 0.0),
    vec3(-0.5,  0.5, 0.0),
    vec3(0.5, -0.5, 0.0),

    // Secondo triangolo
    vec3(-0.5,  0.5, 0.0),
    vec3(0.5,  0.5 ,0.0),
    vec3(0.5, -0.5, 0.0)
);

const vec3 colors[] = vec3[]
(
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0),

    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout (push_constant) uniform PushConstant
{
    mat4 mvp;
} data;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

void main()
{
    gl_Position = data.mvp * vec4(inPosition, 1.0);
    outColor = inColor;
}