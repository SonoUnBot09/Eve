#include "eve/graphics/Buffer.hpp"
#include "eve/graphics/Pass.hpp"
#include "eve/graphics/ShaderHandle.hpp"
#include "eve/graphics/Texture.hpp"
#include "eve/utils/Vec.hpp"
#include <eve/components/Camera.hpp>
#include <eve/entities/SystemRegistrar.hpp>
#include <eve/Debug.hpp>
#include <eve/entities/EntityManager.hpp>
#include <eve/components/Transform.hpp>
#include <eve/graphics/Graphics.hpp>

using namespace Eve::Entities;
using namespace Eve::Graphics;

static ShaderHandle shaderHandle;
static BufferHandle buffer;
static uint64_t elapsedFrames = 0;
static uint32_t elementsCount = 20 * 20;

static Vec2 positions[] = {
    {-0.95f,-0.95f},{-0.85f,-0.95f},{-0.75f,-0.95f},{-0.65f,-0.95f},{-0.55f,-0.95f},
    {-0.45f,-0.95f},{-0.35f,-0.95f},{-0.25f,-0.95f},{-0.15f,-0.95f},{-0.05f,-0.95f},
    { 0.05f,-0.95f},{ 0.15f,-0.95f},{ 0.25f,-0.95f},{ 0.35f,-0.95f},{ 0.45f,-0.95f},
    { 0.55f,-0.95f},{ 0.65f,-0.95f},{ 0.75f,-0.95f},{ 0.85f,-0.95f},{ 0.95f,-0.95f},

    {-0.95f,-0.85f},{-0.85f,-0.85f},{-0.75f,-0.85f},{-0.65f,-0.85f},{-0.55f,-0.85f},
    {-0.45f,-0.85f},{-0.35f,-0.85f},{-0.25f,-0.85f},{-0.15f,-0.85f},{-0.05f,-0.85f},
    { 0.05f,-0.85f},{ 0.15f,-0.85f},{ 0.25f,-0.85f},{ 0.35f,-0.85f},{ 0.45f,-0.85f},
    { 0.55f,-0.85f},{ 0.65f,-0.85f},{ 0.75f,-0.85f},{ 0.85f,-0.85f},{ 0.95f,-0.85f},

    {-0.95f,-0.75f},{-0.85f,-0.75f},{-0.75f,-0.75f},{-0.65f,-0.75f},{-0.55f,-0.75f},
    {-0.45f,-0.75f},{-0.35f,-0.75f},{-0.25f,-0.75f},{-0.15f,-0.75f},{-0.05f,-0.75f},
    { 0.05f,-0.75f},{ 0.15f,-0.75f},{ 0.25f,-0.75f},{ 0.35f,-0.75f},{ 0.45f,-0.75f},
    { 0.55f,-0.75f},{ 0.65f,-0.75f},{ 0.75f,-0.75f},{ 0.85f,-0.75f},{ 0.95f,-0.75f},

    {-0.95f,-0.65f},{-0.85f,-0.65f},{-0.75f,-0.65f},{-0.65f,-0.65f},{-0.55f,-0.65f},
    {-0.45f,-0.65f},{-0.35f,-0.65f},{-0.25f,-0.65f},{-0.15f,-0.65f},{-0.05f,-0.65f},
    { 0.05f,-0.65f},{ 0.15f,-0.65f},{ 0.25f,-0.65f},{ 0.35f,-0.65f},{ 0.45f,-0.65f},
    { 0.55f,-0.65f},{ 0.65f,-0.65f},{ 0.75f,-0.65f},{ 0.85f,-0.65f},{ 0.95f,-0.65f},

    {-0.95f,-0.55f},{-0.85f,-0.55f},{-0.75f,-0.55f},{-0.65f,-0.55f},{-0.55f,-0.55f},
    {-0.45f,-0.55f},{-0.35f,-0.55f},{-0.25f,-0.55f},{-0.15f,-0.55f},{-0.05f,-0.55f},
    { 0.05f,-0.55f},{ 0.15f,-0.55f},{ 0.25f,-0.55f},{ 0.35f,-0.55f},{ 0.45f,-0.55f},
    { 0.55f,-0.55f},{ 0.65f,-0.55f},{ 0.75f,-0.55f},{ 0.85f,-0.55f},{ 0.95f,-0.55f},

    {-0.95f,-0.45f},{-0.85f,-0.45f},{-0.75f,-0.45f},{-0.65f,-0.45f},{-0.55f,-0.45f},
    {-0.45f,-0.45f},{-0.35f,-0.45f},{-0.25f,-0.45f},{-0.15f,-0.45f},{-0.05f,-0.45f},
    { 0.05f,-0.45f},{ 0.15f,-0.45f},{ 0.25f,-0.45f},{ 0.35f,-0.45f},{ 0.45f,-0.45f},
    { 0.55f,-0.45f},{ 0.65f,-0.45f},{ 0.75f,-0.45f},{ 0.85f,-0.45f},{ 0.95f,-0.45f},

    {-0.95f,-0.35f},{-0.85f,-0.35f},{-0.75f,-0.35f},{-0.65f,-0.35f},{-0.55f,-0.35f},
    {-0.45f,-0.35f},{-0.35f,-0.35f},{-0.25f,-0.35f},{-0.15f,-0.35f},{-0.05f,-0.35f},
    { 0.05f,-0.35f},{ 0.15f,-0.35f},{ 0.25f,-0.35f},{ 0.35f,-0.35f},{ 0.45f,-0.35f},
    { 0.55f,-0.35f},{ 0.65f,-0.35f},{ 0.75f,-0.35f},{ 0.85f,-0.35f},{ 0.95f,-0.35f},

    {-0.95f,-0.25f},{-0.85f,-0.25f},{-0.75f,-0.25f},{-0.65f,-0.25f},{-0.55f,-0.25f},
    {-0.45f,-0.25f},{-0.35f,-0.25f},{-0.25f,-0.25f},{-0.15f,-0.25f},{-0.05f,-0.25f},
    { 0.05f,-0.25f},{ 0.15f,-0.25f},{ 0.25f,-0.25f},{ 0.35f,-0.25f},{ 0.45f,-0.25f},
    { 0.55f,-0.25f},{ 0.65f,-0.25f},{ 0.75f,-0.25f},{ 0.85f,-0.25f},{ 0.95f,-0.25f},

    {-0.95f,-0.15f},{-0.85f,-0.15f},{-0.75f,-0.15f},{-0.65f,-0.15f},{-0.55f,-0.15f},
    {-0.45f,-0.15f},{-0.35f,-0.15f},{-0.25f,-0.15f},{-0.15f,-0.15f},{-0.05f,-0.15f},
    { 0.05f,-0.15f},{ 0.15f,-0.15f},{ 0.25f,-0.15f},{ 0.35f,-0.15f},{ 0.45f,-0.15f},
    { 0.55f,-0.15f},{ 0.65f,-0.15f},{ 0.75f,-0.15f},{ 0.85f,-0.15f},{ 0.95f,-0.15f},

    {-0.95f,-0.05f},{-0.85f,-0.05f},{-0.75f,-0.05f},{-0.65f,-0.05f},{-0.55f,-0.05f},
    {-0.45f,-0.05f},{-0.35f,-0.05f},{-0.25f,-0.05f},{-0.15f,-0.05f},{-0.05f,-0.05f},
    { 0.05f,-0.05f},{ 0.15f,-0.05f},{ 0.25f,-0.05f},{ 0.35f,-0.05f},{ 0.45f,-0.05f},
    { 0.55f,-0.05f},{ 0.65f,-0.05f},{ 0.75f,-0.05f},{ 0.85f,-0.05f},{ 0.95f,-0.05f},

    {-0.95f,0.05f},{-0.85f,0.05f},{-0.75f,0.05f},{-0.65f,0.05f},{-0.55f,0.05f},
    {-0.45f,0.05f},{-0.35f,0.05f},{-0.25f,0.05f},{-0.15f,0.05f},{-0.05f,0.05f},
    { 0.05f,0.05f},{ 0.15f,0.05f},{ 0.25f,0.05f},{ 0.35f,0.05f},{ 0.45f,0.05f},
    { 0.55f,0.05f},{ 0.65f,0.05f},{ 0.75f,0.05f},{ 0.85f,0.05f},{ 0.95f,0.05f},

    {-0.95f,0.15f},{-0.85f,0.15f},{-0.75f,0.15f},{-0.65f,0.15f},{-0.55f,0.15f},
    {-0.45f,0.15f},{-0.35f,0.15f},{-0.25f,0.15f},{-0.15f,0.15f},{-0.05f,0.15f},
    { 0.05f,0.15f},{ 0.15f,0.15f},{ 0.25f,0.15f},{ 0.35f,0.15f},{ 0.45f,0.15f},
    { 0.55f,0.15f},{ 0.65f,0.15f},{ 0.75f,0.15f},{ 0.85f,0.15f},{ 0.95f,0.15f},

    {-0.95f,0.25f},{-0.85f,0.25f},{-0.75f,0.25f},{-0.65f,0.25f},{-0.55f,0.25f},
    {-0.45f,0.25f},{-0.35f,0.25f},{-0.25f,0.25f},{-0.15f,0.25f},{-0.05f,0.25f},
    { 0.05f,0.25f},{ 0.15f,0.25f},{ 0.25f,0.25f},{ 0.35f,0.25f},{ 0.45f,0.25f},
    { 0.55f,0.25f},{ 0.65f,0.25f},{ 0.75f,0.25f},{ 0.85f,0.25f},{ 0.95f,0.25f},

    {-0.95f,0.35f},{-0.85f,0.35f},{-0.75f,0.35f},{-0.65f,0.35f},{-0.55f,0.35f},
    {-0.45f,0.35f},{-0.35f,0.35f},{-0.25f,0.35f},{-0.15f,0.35f},{-0.05f,0.35f},
    { 0.05f,0.35f},{ 0.15f,0.35f},{ 0.25f,0.35f},{ 0.35f,0.35f},{ 0.45f,0.35f},
    { 0.55f,0.35f},{ 0.65f,0.35f},{ 0.75f,0.35f},{ 0.85f,0.35f},{ 0.95f,0.35f},

    {-0.95f,0.45f},{-0.85f,0.45f},{-0.75f,0.45f},{-0.65f,0.45f},{-0.55f,0.45f},
    {-0.45f,0.45f},{-0.35f,0.45f},{-0.25f,0.45f},{-0.15f,0.45f},{-0.05f,0.45f},
    { 0.05f,0.45f},{ 0.15f,0.45f},{ 0.25f,0.45f},{ 0.35f,0.45f},{ 0.45f,0.45f},
    { 0.55f,0.45f},{ 0.65f,0.45f},{ 0.75f,0.45f},{ 0.85f,0.45f},{ 0.95f,0.45f},

    {-0.95f,0.55f},{-0.85f,0.55f},{-0.75f,0.55f},{-0.65f,0.55f},{-0.55f,0.55f},
    {-0.45f,0.55f},{-0.35f,0.55f},{-0.25f,0.55f},{-0.15f,0.55f},{-0.05f,0.55f},
    { 0.05f,0.55f},{ 0.15f,0.55f},{ 0.25f,0.55f},{ 0.35f,0.55f},{ 0.45f,0.55f},
    { 0.55f,0.55f},{ 0.65f,0.55f},{ 0.75f,0.55f},{ 0.85f,0.55f},{ 0.95f,0.55f},

    {-0.95f,0.65f},{-0.85f,0.65f},{-0.75f,0.65f},{-0.65f,0.65f},{-0.55f,0.65f},
    {-0.45f,0.65f},{-0.35f,0.65f},{-0.25f,0.65f},{-0.15f,0.65f},{-0.05f,0.65f},
    { 0.05f,0.65f},{ 0.15f,0.65f},{ 0.25f,0.65f},{ 0.35f,0.65f},{ 0.45f,0.65f},
    { 0.55f,0.65f},{ 0.65f,0.65f},{ 0.75f,0.65f},{ 0.85f,0.65f},{ 0.95f,0.65f},

    {-0.95f,0.75f},{-0.85f,0.75f},{-0.75f,0.75f},{-0.65f,0.75f},{-0.55f,0.75f},
    {-0.45f,0.75f},{-0.35f,0.75f},{-0.25f,0.75f},{-0.15f,0.75f},{-0.05f,0.75f},
    { 0.05f,0.75f},{ 0.15f,0.75f},{ 0.25f,0.75f},{ 0.35f,0.75f},{ 0.45f,0.75f},
    { 0.55f,0.75f},{ 0.65f,0.75f},{ 0.75f,0.75f},{ 0.85f,0.75f},{ 0.95f,0.75f},

    {-0.95f,0.85f},{-0.85f,0.85f},{-0.75f,0.85f},{-0.65f,0.85f},{-0.55f,0.85f},
    {-0.45f,0.85f},{-0.35f,0.85f},{-0.25f,0.85f},{-0.15f,0.85f},{-0.05f,0.85f},
    { 0.05f,0.85f},{ 0.15f,0.85f},{ 0.25f,0.85f},{ 0.35f,0.85f},{ 0.45f,0.85f},
    { 0.55f,0.85f},{ 0.65f,0.85f},{ 0.75f,0.85f},{ 0.85f,0.85f},{ 0.95f,0.85f},

    {-0.95f,0.95f},{-0.85f,0.95f},{-0.75f,0.95f},{-0.65f,0.95f},{-0.55f,0.95f},
    {-0.45f,0.95f},{-0.35f,0.95f},{-0.25f,0.95f},{-0.15f,0.95f},{-0.05f,0.95f},
    { 0.05f,0.95f},{ 0.15f,0.95f},{ 0.25f,0.95f},{ 0.35f,0.95f},{ 0.45f,0.95f},
    { 0.55f,0.95f},{ 0.65f,0.95f},{ 0.75f,0.95f},{ 0.85f,0.95f},{ 0.95f,0.95f}
};

void Start(uint32_t systemId)
{    
    ShaderInfo shaderInfo
    {
        .ShaderModule = "triangle",
        .Topology = Topology::TOPOLOGY_TRIANGLE_LIST,
        .PolygonMode = PolygonMode::POLYGON_MODE_FILL,
        .CullMode = CullMode::CULL_MODE_NONE,
        .LineWidth = 1,
        .DepthTest = false,
        .DepthWrite = false,
        .StencilTest = false,
        .CompareOp = DepthTest::DEPTH_COMPARE_LESS,
        .ColorFormat = Format::FORMAT_R8G8B8A8_SRGB
    };

    shaderHandle = Graphics::CreateGraphicsShader(shaderInfo);

    buffer = Graphics::CreateGPUBuffer(sizeof(Vec2) * elementsCount);

    TransferPass pass {};
    pass.UploadBuffer(&positions[0], buffer, sizeof(Vec2) * elementsCount, 0);

    Graphics::AddPass(pass);
}

void Update(float deltaTime, uint32_t systemId)
{
    Vec2Int windowSize = Graphics::GetWindowSize();

    TransientTextureInfo2D textureInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .Format = Format::FORMAT_R8G8B8A8_SRGB
    };

    TransientTextureHandle handle = Graphics::RequestTransientTexture2D(textureInfo);

    GraphicsPass pass {};
    
    pass.UseBufferReadOnlyVertex(buffer);
    
    LoadStoreOp loadStoreOp
    {
        .loadOp = LoadOperation::CLEAR,
        .storeOp = StoreOperation::STORE,
        .clearColor {0,0,0}
    };

    pass.UseColorTarget(handle, loadStoreOp);

    float time = static_cast<float>(elapsedFrames);
    struct PushConstant
    {
        Vec2 resolution;
        float time;
        uint32_t bufferId;
    } pushConstant;

    pushConstant.time = time;
    pushConstant.resolution.x = (float)windowSize.x;
    pushConstant.resolution.y = (float)windowSize.y;
    pushConstant.bufferId = buffer.Id;
    
    pass.DrawInstanced(6, shaderHandle, elementsCount, &pushConstant, Words32(0), Words32(4));

    Graphics::AddPass(pass);

    Graphics::SetPresentTexture(handle);

    elapsedFrames++;
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);