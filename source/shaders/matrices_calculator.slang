module matrices_calculator;

struct PushConstant
{
    uint64_t DrawComputeParamsBufferOffset;
    uint32_t GlobalInstanceOffsetID;
    uint32_t MaterialBufferID;
    uint32_t InstanceParamsBufferID;
    uint32_t DrawComputeParamsBufferID;
    uint32_t RenderViewBufferID;
    uint32_t RenderViewID;
};

[push_constant]
PushConstant pushConstant;

struct Transform
{
    float3 Position;
    float4 Rotation;
    float3 Scale;
};

struct ObjectParams
{
    float4x4 ObjectToWorld;
    float4x4 WorldToObject;
};

struct ComputeParams
{
    uint32_t InstanceCount;
    uint32_t TransformBufferID;
    uint32_t MatricesBufferID;
};

[vk::binding(9, 0)] StructuredBuffer<uint64_t> globalBufferAddresses;

inline T *GetBuffer<T>(uint index) { return (T *)globalBufferAddresses[index]; }

float4x4 TransformToMatrix(Transform t)
{
    float x = t.Rotation.x;
    float y = t.Rotation.y;
    float z = t.Rotation.z;
    float w = t.Rotation.w;

    float3x3 rot;
    rot[0][0] = 1.0f - 2.0f * (y * y + z * z);
    rot[0][1] = 2.0f * (x * y - z * w);
    rot[0][2] = 2.0f * (x * z + y * w);

    rot[1][0] = 2.0f * (x * y + z * w);
    rot[1][1] = 1.0f - 2.0f * (x * x + z * z);
    rot[1][2] = 2.0f * (y * z - x * w);

    rot[2][0] = 2.0f * (x * z - y * w);
    rot[2][1] = 2.0f * (y * z + x * w);
    rot[2][2] = 1.0f - 2.0f * (x * x + y * y);

    float4x4 m = float4x4(0.0f);

    m[0][0] = rot[0][0] * t.Scale.x;
    m[0][1] = rot[0][1] * t.Scale.y;
    m[0][2] = rot[0][2] * t.Scale.z;
    m[0][3] = t.Position.x;

    m[1][0] = rot[1][0] * t.Scale.x;
    m[1][1] = rot[1][1] * t.Scale.y;
    m[1][2] = rot[1][2] * t.Scale.z;
    m[1][3] = t.Position.y;

    m[2][0] = rot[2][0] * t.Scale.x;
    m[2][1] = rot[2][1] * t.Scale.y;
    m[2][2] = rot[2][2] * t.Scale.z;
    m[2][3] = t.Position.z;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;

    return m;
}

float4x4 InvertMatrix(float4x4 m)
{
    float3 col0 = float3(m[0][0], m[1][0], m[2][0]);
    float3 col1 = float3(m[0][1], m[1][1], m[2][1]);
    float3 col2 = float3(m[0][2], m[1][2], m[2][2]);

    float3 invScaleSq;
    invScaleSq.x = 1.0f / dot(col0, col0);
    invScaleSq.y = 1.0f / dot(col1, col1);
    invScaleSq.z = 1.0f / dot(col2, col2);

    float3 invRow0 = col0 * invScaleSq.x;
    float3 invRow1 = col1 * invScaleSq.y;
    float3 invRow2 = col2 * invScaleSq.z;

    float3 translation = float3(m[0][3], m[1][3], m[2][3]);

    float4x4 invM = float4x4(0.0f);

    invM[0][0] = invRow0.x; invM[0][1] = invRow0.y; invM[0][2] = invRow0.z;
    invM[1][0] = invRow1.x; invM[1][1] = invRow1.y; invM[1][2] = invRow1.z;
    invM[2][0] = invRow2.x; invM[2][1] = invRow2.y; invM[2][2] = invRow2.z;

    invM[0][3] = -dot(invRow0, translation);
    invM[1][3] = -dot(invRow1, translation);
    invM[2][3] = -dot(invRow2, translation);

    invM[3][0] = 0.0f;
    invM[3][1] = 0.0f;
    invM[3][2] = 0.0f;
    invM[3][3] = 1.0f;

    return invM;
}

[shader("compute")]
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    ComputeParams params = *(ComputeParams*)(GetBuffer<uint8_t>(pushConstant.DrawComputeParamsBufferID) + pushConstant.DrawComputeParamsBufferOffset);

    uint32_t index = id.x;

    if (index >= params.InstanceCount) { return; }
    
    Transform transform = GetBuffer<Transform>(params.TransformBufferID)[index];
    ObjectParams* objectParamsBuffer = GetBuffer<ObjectParams>(params.MatricesBufferID);

    float4x4 objectToWorld = TransformToMatrix(transform);
    float4x4 worldToObject = InvertMatrix(objectToWorld);

    objectParamsBuffer[index].ObjectToWorld = objectToWorld;
    objectParamsBuffer[index].WorldToObject = worldToObject;
}