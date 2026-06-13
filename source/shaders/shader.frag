#version 460

layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUv;

layout (location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(texSampler, inUv);
}