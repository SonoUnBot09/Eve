module swapchain;

[vk_binding(2, 0)] Texture2D textures[];
[vk_binding(8, 0)] SamplerState samplers[];

struct V2F
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PushConstant
{
    uint32_t textureId;
    uint32_t samplerId;
};

[push_constant]
PushConstant pushConstant;

[shader("vertex")]
V2F vertex(uint32_t vertexID : SV_VertexID)
{
    const float3 vertices[] =
    {
        { -1, -1, 0 },
        { -1, 1, 0 },
        { 1, -1, 0 },
        { 1, 1, 0 }
    };
    const float2 uvs[] =
    {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 }
    };
    const uint32_t indicies[] =
    {
        0, 1, 2,
        1, 2, 3
    };

    V2F output;
    uint32_t vertexIndex = indicies[vertexID];
    output.position = float4(vertices[vertexIndex], 1.0);
    output.uv = uvs[vertexIndex];
    return output;
}


[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    return textures[pushConstant.textureId].Sample(samplers[pushConstant.samplerId], input.uv);
}