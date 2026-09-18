# **Shaders:**

There are two kind of shaders, graphics shaders and compute shaders, you can create them with the following methods in the `Graphics` class in `eve/graphics/Graphics.hpp`:

- `CreateGraphicsShader(ShaderInfo shaderInfo)`
- `CreateComputeShader(std::string module)`

for both Graphics and compute shaders the string module is the name of the shaders. Eve will search the shader with the name specified in all search shader paths specified in the struct `EveEngineCreateInfo` at the startup of the engine, to see more go to `docs/startup.md`

With a graphics shader you can then create a material with the following method:

- `CreateMaterial(ShaderHandle handle)`

You can then use this material to render objects on screen using the draw method for the `Render Graph` to see more go to `render_graph.md`

## **Shaders In Eve:**

Eve uses a shader language called Slang, it is similar to HLSL but with more features. To know more about Slang click [here](https://shader-slang.org/)

To simplify buffer access and other stuff in Eve there is a slang header to help handle and get textures and buffers since Eve uses a bindless approach. You can find this helper in `eve/shaders/eve.slangh`. You can use you those helper function by including the `eve.slangh` file into your shaders and accessing the member `Eve.` and calling the helper methods. Here is a graphics shader example:

    module triangle;

    #include "eve.slangh"

    struct V2F
    {
        float4 position : SV_Position;
        float3 color : COLOR;
    };

    struct Properties
    {
        float3 color;
    }

    struct DrawParams
    {
        uint32_t vertexBufferId;
        uint32_t indexBufferId;
    }


    [shader("vertex")]
    V2F vertex(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID)
    {
        uint32_t globalInstanceID = Eve.GetGlobalInstanceID(instanceID);

        float4x4 model = Eve.GetObjectToWorldMatrix(globalInstanceID);

        Properties properties = Eve.GetMaterialProperties<Properties>();

        DrawParams drawParams = Eve.GetDrawInfoParams<DrawParams>();
        
        float3* positions = Eve.GetBuffer<float3>(drawParams.vertexBufferId);

        uint32_t* indices = Eve.GetBuffer<uint32_t>(drawParams.indexBufferId);

        V2F output;

        uint32_t vertexIndex = indices[vertexID];

        float3 vertexPos = positions[vertexIndex];

        float4x4 mvp = mul(Eve.GetVPMatrix(), model);

        output.position = mul(mvp, float4(vertexPos * 0.25f, 1.0));
        output.color = properties.color;

        return output;
    }

    [shader("fragment")]
    float4 fragment(V2F input) : SV_Target
    {
        return float4(input.color, 1.0);
    }

## **Eve Shader Helper Methods:**

- `GetReadOnlyTexture1D(uint32_t index)`
- `GetReadWriteTexture1D(uint32_t index)`
- `GetReadOnlyTexture2D(uint32_t index)`
- `GetReadWriteTexture2D(uint32_t index)`
- `GetReadOnlyTexture3D(uint32_t index)`
- `GetReadWriteTexture3D(uint32_t index)`
- `GetReadOnlyTextureCube(uint32_t index)`
- `GetReadWriteTextureCube(uint32_t index)`
- `GetSampler(uint32_t index)`
- `GetBuffer<T>(uint32_t index)`

- `GetGlobalInstanceOffsetID()`
- `GetGlobalInstanceID(uint32_t instanceID)`

- `GetMaterialProperties<T>()`
- `GetMaterialPropertiesPtr<T>()`

- `GetObjectToWorldMatrix(uint32_t globalInstanceID)`
- `GetWorldToObjectMatrix(uint32_t globalInstanceID)`

- `GetDrawComputeParams<T>()`
- `GetDrawComputeParamsPtr<T>()`

- `GetViewMatrix()`
- `GetInvViewMatrix()`
- `GetProjMatrix()`
- `GetInvProjMatrix()`
- `GetVPMatrix()`
- `GetInvVPMatrix()`
- `GetViewPosition()`
- `GetViewNearPlane()`
- `GetViewFarPlane()`

---

### **Resource Getters:**

The first methods are used to get buffers and textures given their `ID` which correspond to `TextureHandle.Id`, `BufferHandle.Id`, `TransientTextureHandle.Id`, `TransientBufferHandle.Id` and `SamplerHandle.Id` on the CPU side.

### **Instance ID:**

Every instance in Eve has an ID, each draw call has an offset that point to the start of the draw call instances IDs. You can get that offset by calling `GetObjectToWorldMatrix(uint32_t globalInstanceID)`. You can get the single global instance ID by calling `GetGlobalInstanceID(uint32_t instanceID)` where `instanceID` correspond to `SV_InstanceID` on the shader.

### **Materials:**

Materials in Eve are defined with a struct, that struct need to be exactly called `Properties`, when compiling the shaders Eve will search for this struct. You get the values set to that struct on CPU side by calling `GetMaterialProperties<T>()` or `GetMaterialPropertiesPtr<T>()` on the shader.

### **Object To World & World To Object Matrices:**

`GetObjectToWorldMatrix(uint32_t globalInstanceID)` and `GetWorldToObjectMatrix(uint32_t globalInstanceID)` methods allows to access to the instance model matrix and the inverse model matrix passing the params `uint32_t globalInstanceID` you can get by calling `GetGlobalInstanceID(uint32_t instanceID)` or manually by doing: `instanceId + GetGlobalInstanceOffsetID()` where `instanceId` is `SV_InstanceID`.

### **Draw & Compute Params:**

`DrawParams` and `ComputeParams` can be get by calling - `GetDrawComputeParams<T>()` or `GetDrawComputeParamsPtr<T>()`

### **Render View Data:**

Render View Data like matrices can be obtained by calling `GetViewMatrix()`, `GetInvViewMatrix()`, `GetProjMatrix()`, `GetInvProjMatrix()`, `GetVPMatrix()`, `GetInvVPMatrix()`, `GetViewPosition()`, `GetViewNearPlane()` and `GetViewFarPlane()`. `Inv` stands for inverse and `VP` stands for view projection.