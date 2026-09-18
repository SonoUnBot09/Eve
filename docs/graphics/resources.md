# **Persistent vs Transient:**

The different between persistent and transient resources is a lot important to undestand to use Eve. Both textures and buffers can be persistent and transient.

## **Persistent Resources:**

Persistent resources are created with the following methods in the `Graphics` class in `eve/graphics/Graphics.hpp`:

- `CreateTexture1D(TextureInfo1D textureInfo)`
- `CreateTexture2D(TextureInfo2D textureInfo)`
- `CreateTexture3D(TextureInfo3D textureInfo)`
- `CreateTextureCube(TextureInfo2D textureInfo)`
- `CreateSampler(SamplerInfo samplerInfo)`
- `CreateGPUBuffer(uint64_t size)`

Persistent resources are unique, they are single in the entire application, example:

    TextureHandle myTexture = CreateTexture2D(textureInfo);

Now exists a texture, that you created and can survive between more frames. You can also decide when to destroy this texture. If you do not delete the texture at the end of the program this is not a problem, Eve will destroy all the remaining resources alive. You can destroy persistent resources with those methods in the `Graphics` class:

- `DestroyBuffer(BufferHandle handle)`
- `DestroyTexture(TextureHandle handle)`
- `DestroySampler(SamplerHandle handle)`

## **Transient Resources:**

Transient resources are create with the following methods in the `Graphics` class in `eve/graphics/Graphics.hpp`:

- `RequestTransientTexture1D(TransientTextureInfo1D textureInfo)`
- `RequestTransientTexture2D(TransientTextureInfo2D textureInfo)`
- `RequestTransientTexture3D(TransientTextureInfo3D textureInfo)`
- `RequestTransientTextureCube(TransientTextureInfo2D textureInfo)`
- `RequestTransientBuffer(uint64_t size)`  

Transient resources are not managed completely by the user, the user can only choose when to use those resources in a frame and when to create them. Those resources are managed by the Render Graph which can choose to use the same memory space used by another transient resource. Transient resources are valid only in a single frame.


## **WARNING**:

- Using an already destroyed persistent resource will cause an **UB**.
- Any usage of a transient resource on a different frame will cause an **UB**. 

To see more about texture usage and `Render Graph` go to `render_graph.md`.