module triangle;

#include "eve_resources.slangh"

struct V2F
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PushConstant
{
    float2 resolution;
    float time;
    uint32_t bufferId;
};

[push_constant]
PushConstant pushConstant;


[shader("vertex")]
V2F vertex(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID)
{
    const float3 vertices[] =
    {
        { -1, -1, 0 },
        { -1, 1, 0 },
        { 1, -1, 0 },
        { 1, 1, 0}
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

    float2* positions = GetBuffer<float2>(pushConstant.bufferId);

    float2 basePos = positions[instanceID];

    uint32_t vertexIndex = indicies[vertexID];

    float3 vertexPos = vertices[vertexIndex];
    
    vertexPos = float3(vertexPos.x * 0.1, vertexPos.y * 0.1, 0);
    /*vertexPos.x += basePos.x;
    vertexPos.y += basePos.y;*/

    float2 uv = uvs[vertexIndex];

    output.position = float4(vertexPos, 1.0);
    output.uv = uv;

    return output;
}

[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    return float4(input.uv, 0.25, 1);
}