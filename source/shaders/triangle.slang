module triangle;

struct V2F
{
    float4 position : SV_Position;
};

struct PushConstant
{
    uint32_t textureId;
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

    const uint32_t indicies[] =
    {
        0, 1, 2,
        1, 2, 3
    };

    V2F output;
    uint32_t vertexIndex = indicies[vertexID];
    output.position = float4(vertices[vertexIndex], 1.0);
    return output;
}


[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    return float4(1.0, 0.4, 0.25, 1.0);
}