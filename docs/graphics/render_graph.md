# **Render Graph:**

The `Render Graph` is a part of the Eve's codebase that is responsible to send organize, syncronize and send command to the GPU. Everything in the `Render Graph` is organized by passes, any usage of resources on the GPU must be declared in the passes on the CPU, there are three kind of passes:

- `Graphics Passes`
- `Transfer Passes`
- `Compute Passes`

## **Graphics Pass:**

A `Graphics Pass` is responsible to render into a texture something. A `Graphics Pass` has two methods to draw:

- `Draw(uint32_t vertexShaderInvocations, const Transform& transform, MaterialHandle material, RenderViewHandle renderView, DrawParams* drawInfo)`

- `DrawInstanced(uint32_t vertexShaderInvocations, uint32_t instanceCount, const Transform* transforms, MaterialHandle material, RenderViewHandle renderView,  DrawParams* drawInfo)`

---

The different between these two methods is that the first one does a single draw call, the second one does an instanced draw call with the number of instance equal to `instanceCount`.

**WARNING:** `transforms` needs to have the same number of transform as `instanceCount` otherwise the program will read corrupted memory.

`drawInfo` are custom parameters user can send to the shader so that it can read and use them. To see more about shaders in Eve go to `shaders.md`

To set textures to render onto you can use those methods:

- `UseColorTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)`
- `UseDepthStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)`
- `UseDepthTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)`
- `UseStencilTarget(TransientTextureHandle texture, LoadStoreOp loadStoreOp)`

---

`loadStoreOp` is a struct which describes load and store operations. load operations are executed before starting to execute draw calls, store operations are executed when all draw calls have finished executing, at the end of the pass.

**WARNINGS:** 
- If the shader uses only a color target you must set only a color target, if the shader uses color and depth, you must set color and depth targets. Any mismatch will cause an **UB**.

- Textures format must be the same declared in the shader. Any mismatch will cause an **UB**.

## **Transfer Pass:**

A transfer pass is used to execute copy between resources on the GPU and from the CPU to the GPU with upload methods. Currently, readback is not supported. Available methods:

- `CopyBuffer(TransientBufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0)`
- `CopyTexture(TransientTextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0})`
- `CopyBufferToTexture(TransientBufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`
- `CopyTextureToBuffer(TransientTextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`
- `CopyBuffer(BufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0)`
- `CopyTexture(TextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0})`
- `CopyBufferToTexture(BufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`
- `CopyTextureToBuffer(TextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset, uint32_t BufferRowLength = 0, uint32_t BufferHeightLenght = 0)`

- `CopyBuffer(TransientBufferHandle SrcBuffer, BufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0)`
- `CopyBuffer(BufferHandle SrcBuffer, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t SrcOffset = 0, uint64_t DstOffset = 0)`
- `CopyTexture(TransientTextureHandle SrcTexture, TextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0})`
- `CopyTexture(TextureHandle SrcTexture, TransientTextureHandle DstTexture, glm::ivec3 Extent, glm::ivec3 SrcOffset = {0,0,0}, glm::ivec3 DstOffset = {0,0,0})`

- `CopyBufferToTexture(TransientBufferHandle SrcBuffer, TextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`
- `CopyBufferToTexture(BufferHandle SrcBuffer, TransientTextureHandle DstTexture, uint64_t SrcOffset, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`

- `CopyTextureToBuffer(TransientTextureHandle SrcTexture, BufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset,  uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`
- `CopyTextureToBuffer(TextureHandle SrcTexture, TransientBufferHandle DstBuffer, glm::ivec3 SrcOffset, glm::ivec3 Extent, uint64_t DstOffset,  uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`

- `UploadBuffer(void* SrcData, TransientBufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0)`
- `UploadTexture(void* SrcData, uint64_t Size, TransientTextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`

- `UploadBuffer(void* SrcData, BufferHandle DstBuffer, uint64_t Size, uint64_t DstOffset = 0)`
- `UploadTexture(void* SrcData, uint64_t Size, TextureHandle DstTexture, glm::ivec3 DstOffset, glm::ivec3 Extent, uint32_t BufferRowLength = 0, uint32_t BufferHeightLength = 0)`

## **Compute Pass:**

A `Compute Pass` performs compute operations on the GPU with compute shaders:

- `Dispatch(uint32_t XNumGroups, uint32_t YNumGroups, uint32_t ZNumGroups, ComputeShaderHandle shader, ComputeParams* params)`

`params` allows to send custom data to the GPU and use them in shaders. To see more about shaders go to `shaders.md`.

## **Resource Usage:**

In each pass you need to specify how and which resources you use, if you read from a buffer in the vertex stage you need to specify that, same things for all other cases. You do not need to declare those usage when you use the `Transfer Pass` or when you use textures as render targets in a `Graphic Pass`.

## **Pass Execution Order:**

The order the passes get executed is decide by the order user record passes to the `Render Graph` with the following methods:

- `AddPass(GraphicsPass& pass)`
- `AddPass(TransferPass& pass)`
- `AddPass(ComputePass& pass)`
- `AddPass(GraphicsPass& pass, uint32_t index)`
- `AddPass(TransferPass& pass, uint32_t index)`
- `AddPass(ComputePass& pass, uint32_t index)`

## **Present Texture On Screen:**

To send a texture to the screen you can use the method `SetPresentTexture2D(TransientTextureHandle handle)` which accept only 2D transient textures, any other texture will cause an **UB**.