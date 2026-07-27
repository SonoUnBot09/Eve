#include <Eve/graphics/PassModule.hpp>

using namespace Eve::Graphics;

void GraphicsPass::UseTransientTexture(TransientTextureHandle texture, Usage accessType)
{
    textures.push_back(std::pair{texture, accessType});
}
void GraphicsPass::UseTransientBuffer(TransientBufferHandle buffer, Usage accessType)
{
    buffers.push_back(std::pair{buffer, accessType});
}
void GraphicsPass::DrawMesh(MeshHandle mesh, uint32_t instanceCount)
{
    drawCalls.push_back(std::pair{mesh, instanceCount});
}