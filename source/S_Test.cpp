#include <graphics/RenderGraph.hpp>
#include "Eve/graphics/PassModule.hpp"
#include "Eve/graphics/ShaderHandle.hpp"
#include "Eve/graphics/Texture.hpp"
#include "graphics/helpers/VulkanMapping.hpp"
#include <Eve/components/Camera.hpp>
#include <Eve/Entities/SystemDispatcher.hpp>
#include <Eve/Debug.hpp>
#include <Eve/Entities/EntityManager.hpp>
#include <Eve/components/Transform.hpp>
#include <graphics/registers/ShaderRegistry.hpp>

using namespace Eve::Entities;
using namespace Eve::Graphics;

static ShaderHandle shaderHandle;

void Start()
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
        .samplesCount = TextureSample::SAMPLE_1,
        .ColorFormat = Format::FORMAT_R8G8B8A8_SRGB
    };

    shaderHandle = ShaderRegistry::CreateGraphicsShader(shaderInfo);
}

void Update(const float deltaTime)
{
    Vec2Int windowSize = GraphicsCore::GetWindowSize();

    TransientTextureInfo2D textureInfo
    {
        .Width = static_cast<uint32_t>(windowSize.x),
        .Height = static_cast<uint32_t>(windowSize.y),
        .ArrayLayers = 1,
        .MipLevels = 1,
        .Format = Format::FORMAT_R8G8B8A8_SRGB,
        .Sample = TextureSample::SAMPLE_1
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

    pass.Draw(3, shaderHandle, nullptr, Words32(0), Words32(0));

    RenderGraph::AddPass(pass);

    RenderGraph::SetPresentTexture(handle);
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);