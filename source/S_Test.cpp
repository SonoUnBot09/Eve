#include "eve/graphics/Buffer.hpp"
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

using namespace Eve::Entities;
using namespace Eve::Graphics;
using namespace Eve::Input;

namespace
{
    static MaterialHandle material;
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
            .CullMode = CullMode::CULL_MODE_NONE,
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

        buffer = Graphics::CreateGPUBuffer(256);

        material.SetVector3("color", glm::vec3(0.5,0.7,0));
    }

    void Update(float deltaTime, uint32_t systemId)
    {
        glm::ivec2 windowSize = Graphics::GetWindowSize();

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

        Type cameraComponentType = ComponentsRegistry::GetComponentBit<Camera>();
        Type transformComponentType = ComponentsRegistry::GetComponentBit<Transform>();

        Type componentsRequired = cameraComponentType | transformComponentType;

        QueryInfo queryInfo 
        {
            componentsRequired,
            true
        };

        QueryResult& queryResult = EntityManager::GetTables(queryInfo);

        Table& table = queryResult.GetTable(0);

        Camera& camera = table.GetComponent<Camera>(0, cameraComponentType);

        camera.renderView.SetPerspective(1.22173f, windowSize.x / (float)windowSize.y, 0.1f, 100.0f);

        float time = static_cast<float>(elapsedFrames);
        
        Transform objectTransform1 = 
        {
            {0, 0, 5},
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            {1, 1, 1}
        };

        Transform objectTransform2 = 
        {
            {0, 3, 5},
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            {1, 1, 1}
        };

        std::vector<Transform> transforms {objectTransform1, objectTransform2};

        pass.DrawInstanced(36, 2, *transforms.data(), material, camera.renderView, nullptr);
        //pass.Draw(36, objectTransform1, material, renderView, nullptr);
        //pass.Draw(36, objectTransform2, material, renderView, nullptr);

        Graphics::AddPass(pass);

        Graphics::SetPresentTexture2D(colorTexture);

        elapsedFrames++;
    }
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);