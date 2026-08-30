#include "eve/graphics/Buffer.hpp"
#include "eve/graphics/Pass.hpp"
#include "eve/graphics/ShaderHandle.hpp"
#include "eve/graphics/Texture.hpp"
#include "eve/math/Matrix4x4.hpp"
#include "eve/math/Quaternion.hpp"
#include "eve/math/Vector3.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/matrix.hpp"
#include <eve/components/Camera.hpp>
#include <eve/entities/SystemRegistrar.hpp>
#include <eve/debug/Debug.hpp>
#include <eve/entities/EntityManager.hpp>
#include <eve/components/Transform.hpp>
#include <eve/graphics/Graphics.hpp>
#include <eve/math/Vector2.hpp>
#include <eve/input/Input.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>

using namespace Eve::Entities;
using namespace Eve::Graphics;
using namespace Eve::Math;
using namespace Eve::Input;

static ShaderHandle shaderHandle;
static BufferHandle buffer;
static uint64_t elapsedFrames = 0;
static uint32_t elementsCount = 20 * 20;

void Start(uint32_t systemId)
{    
    ShaderInfo shaderInfo
    {
        .ShaderModule = "triangle",
        .Topology = Topology::TOPOLOGY_TRIANGLE_LIST,
        .PolygonMode = PolygonMode::POLYGON_MODE_FILL,
        .CullMode = CullMode::CULL_MODE_FRONT,
        .LineWidth = 1,
        .DepthTest = true,
        .DepthWrite = true,
        .StencilTest = false,
        .CompareOp = DepthTest::DEPTH_COMPARE_LESS,
        .ColorFormat = Format::FORMAT_R8G8B8A8_SRGB,
        .DepthFormat = Format::FORMAT_D32_SFLOAT
    };

    shaderHandle = Graphics::CreateGraphicsShader(shaderInfo);

    buffer = Graphics::CreateGPUBuffer(256);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,5));
    model = glm::rotate
    (
        model,
        glm::radians(30.0f),
        glm::vec3(0.5, 1, 0)
    );
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective
    (
        glm::radians(60.0f),
        1920 / (float)1080,
        0.1f,
        100.0f
    );

    glm::mat4 projView = projection * view;

    struct Data
    {
        glm::mat4 model;
        glm::mat4 projecView;   
        glm::vec4 color;
    } data;

    data.model = glm::transpose(model);
    data.projecView = glm::transpose(projView);
    data.color = glm::vec4(0.25, 1, 0.25, 1.0);

    TransferPass pass{};

    pass.UploadBuffer(&data, buffer, sizeof(data), 0);

    Graphics::AddPass(pass);
}

void Update(float deltaTime, uint32_t systemId)
{
    Vector2Int windowSize = Graphics::GetWindowSize();

    TransientTextureInfo2D colorInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .Format = Format::FORMAT_R8G8B8A8_SRGB
    };

    TransientTextureInfo2D depthInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .Format = Format::FORMAT_D32_SFLOAT
    };

    TransientTextureHandle colorTexture = Graphics::RequestTransientTexture2D(colorInfo);
    TransientTextureHandle depthTexture = Graphics::RequestTransientTexture2D(depthInfo);

    GraphicsPass pass {};
    
    LoadStoreOp loadStoreOpColor
    {
        .loadOp = LoadOperation::CLEAR,
        .storeOp = StoreOperation::STORE,
        .clearColor {0,0,0}
    };

    LoadStoreOp loadStoreOpDepth
    {
        .loadOp = LoadOperation::CLEAR,
        .storeOp = StoreOperation::DISCARD,
        .clearDepth = 1.0
    };

    pass.UseBufferReadOnlyVertex(buffer);
    pass.UseColorTarget(colorTexture, loadStoreOpColor);
    pass.UseDepthTarget(depthTexture, loadStoreOpDepth);

    /*
    Matrix4x4 model = Matrix4x4::Identity();
    model.TRS({0,0,3}, Quaternion::Identity(), Vector3{1,1,1});

    Matrix4x4 camera = Matrix4x4::Identity();
    camera.TRS(Vector3(0,0,0), Quaternion::Identity(), Vector3(1,1,1));

    Matrix4x4 proj = Matrix4x4::Identity();
    proj.*/

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,5));

    model = glm::rotate
    (
        model,
        glm::radians(30.0f),
        glm::vec3(0.5, 1, 0)
    );

    glm::mat4 view = glm::mat4(1.0f);

    glm::mat4 projection = glm::perspective
    (
        glm::radians(60.0f),
        windowSize.x / (float)windowSize.y,
        0.1f,
        100.0f
    );

    glm::mat4 mvp = projection * view * model;

    float time = static_cast<float>(elapsedFrames);
    struct PushConstant
    {
        uint32_t bufferId;
    } pushConstant;

    pushConstant.bufferId = buffer.Id;
   
    Words32 size = Words32(std::ceil(sizeof(pushConstant) / 4.0f));
    pass.Draw(36, shaderHandle, &pushConstant, Words32(0), Words32(1));

    Graphics::AddPass(pass);

    Graphics::SetPresentTexture(colorTexture);

    elapsedFrames++;
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);