
#include "eve/entities/ComponentsRegistry.hpp"
#include "eve/entities/QueryResult.hpp"
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
static uint64_t elapsedFrames = 0;

void Awake(uint32_t systemId)
{
    ComponentsRegistry::RegisterComponent<Transform>();
    ComponentsRegistry::RegisterComponent<Camera>();

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
    QueryResult& queryResult = EntityManager::GetTables({componentType, true});
    Table& table = queryResult.GetTable(0);

    for (uint32_t i = 0; i < 456; i++)
    {
        Transform& transform = table.GetComponent<Transform>(i, componentType);
        EntityManager::ScheduleDestructionCommand({i, 0}, systemId);
    }

    Camera camera({0,0,1}, {0,1,0},1, 10);

    EntityCommandInfo commandInfo {};
    commandInfo.AddComponent<Camera>(camera);
    EntityManager::ScheduleTransitionCommand({456,0}, commandInfo, systemId);

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
}

void Update(float deltaTime, uint32_t systemId)
{
    Type transformType = ComponentsRegistry::GetComponentBit<Transform>();
    Type cameraType = ComponentsRegistry::GetComponentBit<Camera>();
    Type archtype = ComponentsRegistry::GetComponentMask<Transform, Camera>();

    QueryResult& result = EntityManager::GetTables({archtype, true});
    Table& table = result.GetTable(0);

    SlotInfo slotInfo = table.GetSlotInfo(0);

    Transform& transform = table.GetComponent<Transform>(slotInfo, transformType);
    Camera& camera = table.GetComponent<Camera>(slotInfo, cameraType);
    Entity entity = table.GetEntity(slotInfo);

    /*
    std::cout << "POSITION X: " << transform.Position.x << std::endl;
    std::cout << "CAMERA " << camera.sensitivity << std::endl;
    std::cout << "ENTITY ID:  " << entity.Id << " GENERATION:  " << entity.GeneratationId << std::endl;*/

    Vec2Int windowSize = Graphics::GetWindowSize();

    TransientTextureInfo2D textureInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .Format = Format::FORMAT_R8G8B8A8_SRGB
    };

    TransientTextureHandle handle = Graphics::RequestTransientTexture2D(textureInfo);

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

    Graphics::AddPass(pass);

    Graphics::SetPresentTexture(handle);

    elapsedFrames++;
}

static SystemRegistrar awake(Awake, SystemStage::Awake);
static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);