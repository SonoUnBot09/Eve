module triangle;

struct V2F
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PushConstant
{
    float2 resolution;
    float time;
}

[push_constant]
PushConstant pushConstant;

float2 hash(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in float2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

    float2 i = floor(p + (p.x + p.y) * K1);
    float2 a = p - i + (i.x + i.y) * K2;
    float m = step(a.y, a.x);
    float2 o = float2(m, 1.0 - m);
    float2 b = a - o + K2;
    float2 c = a - 1.0 + 2.0 * K2;
    float3 h = max(0.5 - float3(dot(a, a), dot(b, b), dot(c, c)), 0.0);
    float3 n = h * h * h * h * float3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, float3(70.0));
}

float FractalNoise(float2 uv, float lacunarity, float power, uint32_t octaves)
{
    float frequency = 1;
    float amplitude = 1;

    float noiseValue = 0;
    for (uint32_t i = 0; i < octaves; i++)
    {
        noiseValue += noise(uv * frequency) * amplitude;

        frequency *= lacunarity;
        amplitude *= power;
    }

    return noiseValue;
}

float CalculateColor(float noiseValue, float iso)
{
    if (noiseValue > iso)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

float3 GetViewDir(float fovDeg, float aspectRatio, float pitch, float2 uv)
{
    float fov = radians(fovDeg);
    float sensorHeight = tan(fov * 0.5) * 2;
    float sensorWidth = sensorHeight * aspectRatio;
    float2 sensorPos = (uv - 0.5) * float2(sensorWidth, sensorHeight);

    float3 right = float3(1, 0, 0);
    float3 fwd = float3(0, sin(pitch), cos(pitch));
    float3 up = cross(fwd, right);

    float3 viewDir = right * sensorPos.x + up * sensorPos.y + fwd;
    return normalize(viewDir);
}

[shader("vertex")]
V2F vertex(uint32_t vertexID: SV_VertexID)
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

    uint32_t vertexIndex = indicies[vertexID];

    float2 uv = uvs[vertexIndex];

    output.position = float4(vertices[vertexIndex], 1.0);
    output.uv = uv;

    return output;
}


[shader("fragment")]
float4 fragment(V2F input) : SV_Target
{

    return float4(input.uv, 0.25, 1);
    float aspectRatio = pushConstant.resolution.x / pushConstant.resolution.y;
    float3 viewDir = GetViewDir(70, aspectRatio, 0, input.uv);
    float3 viewPos = float3(0, 10, 0);

    float groundDst = -viewPos.y / viewDir.y;
    float3 groundPos = viewPos + viewDir * groundDst;

    float noiseValue = FractalNoise(groundPos.xz, 2.0, 0.5, 1);
    return noiseValue;
}