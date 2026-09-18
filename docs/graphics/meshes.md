# **Mesh:**

A mesh can be created and destroy with the following methods inside `Graphics` in `eve/graphics/Graphics.hpp`:

- `CreateMesh()`
- `CreateCubeMesh()`
- `CreateUVSphereMesh()`
- `CreateIcoSphereMesh()`
- `CreatePlaneMesh()`
- `CreateQuadMesh()`
- `DestroyMesh(MeshHandle handle)`

---

`CreateMesh()` creates an empty mesh, the other methods create a default mesh. To get the CPU side mesh use the method `GetCPUMesh(MeshHandle handle)` getting the `CPUMesh` allows to change vertices, indicies, normals, tangents and other attributes manually. `MeshHandle` has methods to do that automatically. To upload data on the GPU call `ApplyToGPU()`. 

To get the GPU side mesh use the method `GetGraphicsMesh(MeshHandle handle)` which contains the buffers of the vertices, normals, indices and others attributes that can be sent to the GPU. The buffers's IDs can be sent to the shaders to access them.