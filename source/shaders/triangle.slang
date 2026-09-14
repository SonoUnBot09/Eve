module triangle;

#include "eve.slangh"

struct V2F
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float3 color : COLOR;
    nointerpolation float3 lightDir;
};

struct Properties
{
    float3 color;
}

struct DrawParams
{
    uint32_t vertexBufferId;
    uint32_t indexBufferId;
    uint32_t normalBufferId;
    float3 lightDir;
}


[shader("vertex")]
V2F vertex(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID)
{
    uint32_t globalInstanceID = Eve.GetGlobalInstanceID(instanceID);

    float4x4 model = Eve.GetObjectToWorldMatrix(globalInstanceID);

    Properties properties = Eve.GetMaterialProperties<Properties>();

    DrawParams drawParams = Eve.GetDrawInfoParams<DrawParams>();
    
    float3 *positions = Eve.GetBuffer<float3>(drawParams.vertexBufferId);
    float3 *normals = Eve.GetBuffer<float3>(drawParams.normalBufferId);

    uint32_t *indices = Eve.GetBuffer<uint32_t>(drawParams.indexBufferId);

    V2F output;

    uint32_t vertexIndex = indices[vertexID];

    float3 vertexPos = positions[vertexIndex];
    float3 normalLocal = normals[vertexIndex];

    float3 normal = normalize(mul(model, float4(normalLocal, 0)).xyz);

    float4x4 mvp = mul(Eve.GetVPMatrix(), model);

    output.position = mul(mvp, float4(vertexPos * 0.25f, 1.0));
    output.normal = normal;
    output.color = properties.color;
    output.lightDir = drawParams.lightDir;

    return output;
}

[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{
    float3 normal = normalize(input.normal);

    float3 lightDir = normalize(input.lightDir);

    float lightIntensity = max(dot(normal, lightDir), 0.15);

    return float4(input.color * lightIntensity, 1.0);
}