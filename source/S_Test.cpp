#include "eve/entities/QueryInfo.hpp"
#include "eve/graphics/Buffer.hpp"
#include "eve/graphics/Mesh.hpp"
#include "eve/graphics/Pass.hpp"
#include "eve/graphics/RenderViewHandle.hpp"
#include "eve/graphics/ShaderHandle.hpp"
#include "eve/graphics/Texture.hpp"
#include <eve/components/Camera.hpp>
#include <eve/entities/SystemRegistrar.hpp>
#include <eve/debug/Debug.hpp>
#include <eve/entities/EntityManager.hpp>
#include <eve/components/Transform.hpp>
#include <eve/graphics/Graphics.hpp>
#include <eve/input/Input.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>
#include <eve/graphics/UI.hpp>
#include <eve/window/Window.hpp>

using namespace Eve::Entities;
using namespace Eve::Graphics;
using namespace Eve::Input;
using namespace Eve::Window;

namespace
{
    static MaterialHandle material;
    static BufferHandle buffer;
    static uint64_t elapsedFrames = 0;
    static uint32_t elementsCount = 20 * 20;

    static std::vector<MeshHandle> meshHandles;
    static std::vector<std::string> meshNames { "Cube", "UV Sphere", "Ico Sphere", "Plane", "Quad" };
    static uint32_t meshIndex = 0;

    inline static std::vector<Transform> transforms;
    inline static bool getTransforms = true;

    inline static glm::vec3 color = glm::vec3(0.5,0.7,0);
    inline static glm::vec3 lightDir = glm::vec3(0.5, 1, 1);
    inline static glm::vec3 cubesOffset = glm::vec3(0, 0, 0);
    inline static glm::vec3 cubesRotation = glm::vec3(0,0,0);

    void Start(uint32_t systemId)
    {    
        transforms.reserve(100000);
        ShaderInfo shaderInfo
        {
            .ShaderModule = "triangle",
            .Topology = Topology::TOPOLOGY_TRIANGLE_LIST,
            .PolygonMode = PolygonMode::POLYGON_MODE_FILL,
            .CullMode = CullMode::CULL_MODE_BACK,
            .LineWidth = 1,
            .DepthTest = true,
            .DepthWrite = true,
            .StencilTest = false,
            .CompareOp = DepthTest::DEPTH_COMPARE_LESS,
            .ColorFormat = Format::FORMAT_R8G8B8A8_SRGB,
            .DepthFormat = Format::FORMAT_D32_SFLOAT
        };

        ShaderHandle shaderHandle = Graphics::CreateGraphicsShader(shaderInfo);

        material = Graphics::CreateMaterial(shaderHandle);

        material.SetVector3("color", color);

        meshHandles.push_back(Graphics::CreateCubeMesh());
        meshHandles.push_back(Graphics::CreateUVSphereMesh());
        meshHandles.push_back(Graphics::CreateIcoSphereMesh());
        meshHandles.push_back(Graphics::CreatePlaneMesh());
        meshHandles.push_back(Graphics::CreateQuadMesh());
    }

    void Update(float deltaTime, uint32_t systemId)
    {
        glm::ivec2 windowSize = Window::GetWindowSize();

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

        GraphicsPass pass {1, 100000};
        
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

        pass.UseColorTarget(colorTexture, loadStoreOpColor);
        pass.UseDepthTarget(depthTexture, loadStoreOpDepth);

        Type cameraComponentType = ComponentsRegistry::GetComponentBit<Camera>();
        Type transformComponentType = ComponentsRegistry::GetComponentBit<Transform>();

        Type componentsRequired = cameraComponentType | transformComponentType;

        QueryInfo queryInfo 
        {
            componentsRequired,
            true
        };

        QueryResult& queryResult = EntityManager::GetTables(queryInfo);

        Table& cameraTable = queryResult.GetTable(0);

        Camera& camera = cameraTable.GetComponent<Camera>(0, cameraComponentType);

        camera.renderView.SetPerspective(1.22173f, windowSize.x / (float)windowSize.y, 0.1f, 300.0f);

        float time = static_cast<float>(elapsedFrames);

        QueryInfo objectQueryInfo
        {
            transformComponentType,
            true
        };
        
        QueryResult& tables = EntityManager::GetTables(objectQueryInfo);

        Table& table = tables.GetTable(0);

        uint32_t entitiesCount = table.GetEntitiesCount();

        /*
        for(uint32_t i = 0; i < entitiesCount; i++)
        {
            Transform& transform = table.GetComponent<Transform>(i, transformComponentType);

            pass.Draw(36, transform, material, camera.renderView, nullptr);
        }*/

        
        transforms.clear();
        for(uint32_t i = 0; i < entitiesCount; i++)
        {
            Transform transform = table.GetComponent<Transform>(i, transformComponentType);

            transform.Position += cubesOffset;
            transform.Rotation *= glm::quat(cubesRotation);

            transforms.push_back(transform);
        }
      

        UI::DockableWindow("Scene Properties");

        UI::ColorWheel3("Cubes Color", color);
        UI::Float3("Light Direction", lightDir, -1.0f, 1.0f);
        UI::Float3RGB("Cubes Position", cubesOffset);
        UI::Float3RGB("Cubes Rotation", cubesRotation);
        UI::SelectableList("Mesh", meshNames, meshIndex);

        material.SetVector3("color", color);

        struct MyDrawParams
        {
            uint32_t vertexBufferId;
            uint32_t indexBufferId;
            uint32_t normalBufferId;
            glm::vec3 lightDir;
        } myDrawParams;

        MeshHandle meshHandle = meshHandles[meshIndex];
        GraphicsMesh gfxMesh = Graphics::GetGraphicsMesh(meshHandle);

        myDrawParams.vertexBufferId = gfxMesh.VertexBuffer.Id;
        myDrawParams.indexBufferId = gfxMesh.IndexBuffer.Id;
        myDrawParams.normalBufferId = gfxMesh.NormalBuffer.Id;
        myDrawParams.lightDir = lightDir;

        pass.UseBufferReadOnlyVertex(gfxMesh.VertexBuffer);
        pass.UseBufferReadOnlyVertex(gfxMesh.IndexBuffer);
        pass.UseBufferReadOnlyVertex(gfxMesh.NormalBuffer);

        DrawParams drawParams(&myDrawParams, sizeof(MyDrawParams));

        pass.DrawInstanced(gfxMesh.IndicesCount, entitiesCount, transforms.data(), material, camera.renderView, &drawParams);

        Graphics::AddPass(pass);

        Graphics::SetPresentTexture2D(colorTexture);

        elapsedFrames++;
    }
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);