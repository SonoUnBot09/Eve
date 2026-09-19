# **Eve Engine**

<img width="693" height="445" alt="image" src="https://github.com/user-attachments/assets/27fe442a-1679-41a0-8577-3336eb283cf5" />

## ✨ **Quick Overview:**

Eve is a **open-source**, **cross-platform** (Windows, Linux), **simple** and **optimized graphics framework** written to simplify and learn **Vulkan**. Eve lets easily draw objects on the screen without losing performance and customization.

### 🕹️ **Requirements:**

- OS: Windows, Linux
- Compiler and support for C++20
- CMake (+3.28)
- GPU drivers supporting Vulkan +1.2

### 🪐 **Quick Start:**

First of all to develop in Eve is required the Vulkan SDK +1.2. You can install it [here](https://vulkan.lunarg.com/sdk/home)


To use Eve you need to clone this repository in your PC. You can do that manually, using a git command in you terminal, downloading the code from the releases or CMake Fetch Content.


**Example (CMake Fetch Content):**

```
cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(YourProject LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include(FetchContent)

FetchContent_Declare(
    Eve
    GIT_REPOSITORY https://github.com/SonoUnBot09/Eve
    GIT_TAG        v1.0.0
)

FetchContent_MakeAvailable(Eve)

add_executable(Eve_Testing
    main.cpp
    S_Entity.cpp
    S_Test.cpp
)

target_link_libraries(YourProject PRIVATE Eve)
```
You can also use the Fetch Content directly from the releases (Recommended). To start using Eve check the docs [here](https://github.com/SonoUnBot09/Eve/tree/main/docs). To directly see how you can initialize Eve go [here](https://github.com/SonoUnBot09/Eve/blob/main/docs/startup.md).

## 🚀 **Features:**

- **Render Graph** (automatically handles memory barriers, memory aliasing and resource pooling)
- **Custom ECS** with archtypes
- **Material System**
- **Input System**
- **Immediate UI Support**
- **Compute Shaders**
- **Mesh System**

## 💻 **Code Snippets:**

```cpp
// Register the transform component
ComponentsRegistry::RegisterComponent<Transform>();

EntityCommandInfo entityCommandInfo{};

// Initialize a transform
Transform transform 
{
    {static_cast<float>(x),static_cast<float>(y),static_cast<float>(z)},
    glm::quat(1.0, 0.0f, 0.0f, 0.0f),
    {1,1,1}
};

// Adds the transform component to the entity command info
entityCommandInfo.AddComponent<Transform>(transform);

// Create an entity with the transform component
EntityManager::ScheduleCreationCommand(&entityCommandInfo, systemId);

// Get the the archtype
Type componentsRequired = ComponentsRegistry::GetComponentMask<Transform, Camera>();

QueryInfo queryInfo 
{
    componentsRequired,
    true
};

// Searching through all the archtypes
QueryResult& queryResult = EntityManager::GetTables(queryInfo);

// Get the first archtype found
Table& table = queryResult.GetTable(0);

// Get each component types
Type cameraComponentType = ComponentsRegistry::GetComponentBit<Camera>();
Type transformComponentType = ComponentsRegistry::GetComponentBit<Transform>();

// Get the transform and the entity component from the first entity
Transform& transform = table.GetComponent<Transform>(0, transformComponentType);
Camera& camera = table.GetComponent<Camera>(0, cameraComponentType);

// Create a cube mesh
MeshHandle mesh = Graphics::CreateCubeMesh();

// Get window size
glm::ivec2 windowSize = Window::GetWindowSize();

TransientTextureInfo2D colorInfo
{
    .Width = static_cast<uint32_t>(windowSize.x),
    .Height = static_cast<uint32_t>(windowSize.y),
    .Format = Format::FORMAT_R8G8B8A8_SRGB
};

// Create a transient color texture with the window size (managed by the Render Graph)
TransientTextureHandle colorTexture = Graphics::RequestTransientTexture2D(colorInfo);

// Draw call
pass.Draw(gfxMesh.IndicesCount, transforms.data(), material, camera.renderView, &drawParams);

// Set the texture to be presented on the screen
Graphics::SetPresentTexture2D(colorTexture);
```

## 📸 **Screenshots:**

<img width="1895" height="979" alt="Screenshot 2026-09-07 165640" src="https://github.com/user-attachments/assets/7ae49cdc-9323-4c1f-a4af-7d3d60202bfe" />

<img width="1611" height="875" alt="image" src="https://github.com/user-attachments/assets/4d0a2d6a-4b91-4696-9fee-db032677c850" />


## 🔧 **How It Works**

- Given that **Vulkan** is extremely verbose, even for tasks that appear simple at first glance, I decided to write this framework featuring a Render Graph. The **Render Graph** analyzes the passes the GPU must execute and automatically inserts **barriers** where necessary; it employs **memory aliasing** (where two or more resources share the same VRAM space) to limit memory usage and attempts to reuse resources from previous frames rather than constantly creating new ones, thereby saving a significant amount of time.

- Instead of using a Unity-style approach based on **GameObjects**, and thus relying on the **OOP** (Object Orient Programming) paradigmm I chose to use a Unity DOTS-style **ECS** approach that utilizes **archetypes** and minimizes CPU cache misses.

- To set parameters for shaders to use, I created a **material system** that analyzes the shader and looks for a struct named `Properties`; if found, this struct is interpreted as the one defining the **material properties**. The system allows properties to be set simply by **using their names** as defined in the shader, greatly simplifying the tedious process of managing **UBOs in Vulkan**.

- **Eve** includes a small wrapper around **ImGui** that makes it **extremely simple** and **fast to use** for creating basic UIs.

**To know more on how to use Eve or how it works go [here](https://github.com/SonoUnBot09/Eve/tree/main/docs)**

## 📒 **Resources Used:**
- [Basic Vulkan Tutorial YT](https://www.youtube.com/watch?v=DC9FBRQKNck&t=268s)
- [Render Graph high level introduction](https://www.youtube.com/watch?v=uQQzDRFKm_4)
- [Render Graph Frostbite](https://www.youtube.com/watch?v=1Sb3s7Xie4M&t=2886s)
- [Sebastian Lague On Vulkan](https://www.youtube.com/watch?v=_JGMgpyCTsY)
- [Render Graph 1](https://logins.github.io/graphics/2021/05/31/RenderGraphs.html)
- [Render Graph 2](https://themaister.net/blog/2017/08/15/render-graphs-and-vulkan-a-deep-dive/)
- [Vulkan 1](https://www.howtovulkan.com/)
- [Vulkan 2](https://vkguide.dev/)
- [Unity ECS](https://docs.unity3d.com/Packages/com.unity.entities@0.2/manual/ecs_core.html)
- [SDL3](https://wiki.libsdl.org/SDL3/FrontPage)
