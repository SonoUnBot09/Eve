module triangle;

struct V2F
{
    float4 position : SV_Position;
    float3 color : TEXCOORD0;
};

struct PushConstant
{
    float time;
}

[push_constant]
PushConstant pushConstant;

float3 CalculateColor(uint32_t vertexIndex, float time)
{
    const float PI = 3.14159265359;
    const float TWO_THIRD_PI = (2.0 * PI) / 3.0; // 120 gradi

    // Angolo base basato sul vertice, scalato nel tempo
    float angle = vertexIndex * TWO_THIRD_PI - time;

    // Sfasamento di 120° tra Rosso, Verde e Blu
    float r = max(0.0, cos(angle));
    float g = max(0.0, cos(angle - TWO_THIRD_PI));
    float b = max(0.0, cos(angle - 2.0 * TWO_THIRD_PI));

    return float3(r, g, b);
}

[shader("vertex")]
V2F vertex(uint32_t vertexID : SV_VertexID)
{
    const float3 vertices[] =
    {
        { -0.5, -0.5, 0 },
        { 0, 0.5, 0 },
        { 0.5, -0.5, 0 }
    };

    const float3 colors[] =
    {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }
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
        0, 1, 2
    };

    V2F output;
    uint32_t vertexIndex = indicies[vertexID];
    output.position = float4(vertices[vertexIndex], 1.0);
    output.color = CalculateColor(vertexIndex, pushConstant.time * 0.01);
    return output;
}


[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    return float4(input.color, 1.0);
}