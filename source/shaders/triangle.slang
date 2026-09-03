module triangle;

#include "eve.slangh"

struct V2F
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

struct Properties
{
    float3 color;
}


[shader("vertex")]
V2F vertex(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID)
{
    static const float3 vertices[24] =
    {
        // Front face (Z = 1)
        float3(-1.0, -1.0, 1.0), float3(1.0, -1.0, 1.0), float3(1.0, 1.0, 1.0), float3(-1.0, 1.0, 1.0),
        // Back face (Z = -1)
        float3(1.0, -1.0, -1.0), float3(-1.0, -1.0, -1.0), float3(-1.0, 1.0, -1.0), float3(1.0, 1.0, -1.0),
        // Right face (X = 1)
        float3(1.0, -1.0, 1.0), float3(1.0, -1.0, -1.0), float3(1.0, 1.0, -1.0), float3(1.0, 1.0, 1.0),
        // Left face (X = -1)
        float3(-1.0, -1.0, -1.0), float3(-1.0, -1.0, 1.0), float3(-1.0, 1.0, 1.0), float3(-1.0, 1.0, -1.0),
        // Top face (Y = 1)
        float3(-1.0, 1.0, 1.0), float3(1.0, 1.0, 1.0), float3(1.0, 1.0, -1.0), float3(-1.0, 1.0, -1.0),
        // Bottom face (Y = -1)
        float3(-1.0, -1.0, -1.0), float3(1.0, -1.0, -1.0), float3(1.0, -1.0, 1.0), float3(-1.0, -1.0, 1.0)
    };

    static const float3 normals[24] =
    {
        // Front
        float3(0, 0, 1), float3(0, 0, 1), float3(0, 0, 1), float3(0, 0, 1),
        // Back
        float3(0, 0, -1), float3(0, 0, -1), float3(0, 0, -1), float3(0, 0, -1),
        // Right
        float3(1, 0, 0), float3(1, 0, 0), float3(1, 0, 0), float3(1, 0, 0),
        // Left
        float3(-1, 0, 0), float3(-1, 0, 0), float3(-1, 0, 0), float3(-1, 0, 0),
        // Top
        float3(0, 1, 0), float3(0, 1, 0), float3(0, 1, 0), float3(0, 1, 0),
        // Bottom
        float3(0, -1, 0), float3(0, -1, 0), float3(0, -1, 0), float3(0, -1, 0)
    };

    static const uint32_t indices[36] =
    {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Right
        12, 13, 14, 14, 15, 12, // Left
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };

    const float2 uvs[] =
    {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 }
    };

    uint32_t globalInstanceID = Eve.GetGlobalInstanceID(instanceID);

    float4x4 model = Eve.GetObjectToWorldMatrix(globalInstanceID);

    Properties properties = Eve.GetMaterialProperties<Properties>();

    V2F output;

    uint32_t vertexIndex = indices[vertexID];

    float3 vertexPos = vertices[vertexIndex];
    float3 normalLocal = normals[vertexIndex];

    float3 normal = normalize(mul(model, float4(normalLocal, 0)).xyz);

    float4x4 mvp = mul(Eve.GetVPMatrix(), model);

    output.position = mul(mvp, float4(vertexPos * 0.25f, 1.0));
    output.normal = normal;
    output.color = properties.color;

    return output;
}

[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    float3 normal = normalize(input.normal);

    // Direzione della luce fissa nel mondo
    float3 lightDir = normalize(float3(1.0, 1.0, -1.0));

    float lightIntensity = max(dot(normal, lightDir), 0.15);

    return float4(input.color * lightIntensity, 1.0);
}