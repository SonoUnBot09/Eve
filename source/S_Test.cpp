#include <graphics/RenderGraph.hpp>
#include "eve/entities/ComponentsRegistry.hpp"
#include "eve/graphics/PassModule.hpp"
#include "eve/graphics/ShaderHandle.hpp"
#include "eve/graphics/Texture.hpp"
#include "eve/utils/Vec.hpp"
#include <eve/components/Camera.hpp>
#include <eve/entities/SystemRegistrar.hpp>
#include <eve/Debug.hpp>
#include <eve/entities/EntityManager.hpp>
#include <eve/components/Transform.hpp>
#include <graphics/registers/ShaderRegistry.hpp>

using namespace Eve::Entities;
using namespace Eve::Graphics;

static ShaderHandle shaderHandle;
static uint64_t elapsedFrames = 0;

void Awake(uint32_t systemId)
{
    ComponentsRegistry::RegisterComponent<Transform>();

    Transform transform
    {
        .Position {10,1,0},
        .Rotation {0,0,0},
        .Scale {1,1,1}
    };

    EntityCommandInfo command{};
    for (uint32_t i = 0; i < 457; i++)
    {
        transform.Position.x = i;
        command.AddComponent<Transform>(transform);
        EntityManager::ScheduleCreationCommand(&command, systemId);

        command.Clear();
    }
}

void Start(uint32_t systemId)
{    
    Type componentType = ComponentsRegistry::GetComponentBit<Transform>();
    Table& table = EntityManager::GetTable(componentType);

    for (uint32_t i = 0; i < 457; i++)
    {
        Transform& transform = table.GetComponent<Transform>(i, componentType);
        EntityManager::ScheduleDestructionCommand({i, 0}, systemId);
    }

    Transform transform
    {
        .Position {5,1,0},
        .Rotation {0,0,0},
        .Scale {1,1,1}
    };

    EntityCommandInfo command{};
    command.AddComponent<Transform>(transform);
    EntityManager::ScheduleCreationCommand(&command, systemId);

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
        .samplesCount = TextureSample::SAMPLE_1,
        .ColorFormat = Format::FORMAT_R8G8B8A8_SRGB
    };

    shaderHandle = ShaderRegistry::CreateGraphicsShader(shaderInfo);
}

void Update(float deltaTime, uint32_t systemId)
{
    Type componentType = ComponentsRegistry::GetComponentBit<Transform>();
    Table& table = EntityManager::GetTable(componentType);

    Transform& transform = table.GetComponent<Transform>(0, componentType);

    std::cout << "POSITION X: " << transform.Position.x << std::endl;

    Vec2Int windowSize = GraphicsCore::GetWindowSize();

    TransientTextureInfo2D textureInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = Format::FORMAT_R8G8B8A8_SRGB
    };

    TransientTextureHandle handle = RenderGraph::RequestTransientTexture2D(textureInfo);

    GraphicsPass pass {};

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
    } pushConstant;

    pushConstant.time = time;
    pushConstant.resolution.x = (float)windowSize.x;
    pushConstant.resolution.y = (float)windowSize.y;
    
    pass.Draw(6, shaderHandle, &pushConstant, Words32(0), Words32(3));

    RenderGraph::AddPass(pass);

    RenderGraph::SetPresentTexture(handle);

    elapsedFrames++;
}

static SystemRegistrar awake(Awake, SystemStage::Awake);
static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);