#include <components/Camera.hpp>
#include <Eve/SystemDispatcher.hpp>
#include <Eve/Debug.hpp>
#include <Eve/EntityManager.hpp>
#include <components/Transform.hpp>
#include <SDL3/SDL.h>

using namespace Eve::Entities;

Type transformComponentType = 0;
Type cameraComponentType = 0;
float accumulator = 0;

void Start()
{
    // register components
    transformComponentType.set(0);
    cameraComponentType.set(1);
    uint32_t transformComponentTicket = ComponentsRegistry::RegisterComponent<Transform>();
    uint32_t cameraComponentTicket = ComponentsRegistry::RegisterComponent<Camera>();

    EntityCommandInfo entityCommandInfo(0, 0);

    uint32_t commandPoolId = EntityManager::CreateCommandPool(0, 0, 0, 0 * 36, 0);
    EntityCommandPool& commandPool = EntityManager::GetEntityCommandPool(commandPoolId);

    uint32_t id = 0;
    for(int32_t x = 0; x < 46; x++)
    {
        for(int32_t y = 0; y < 46; y++)
        {
            for (int32_t z = 0; z < 46; z++)
            {
                Entity entity = {id, 0};
                Transform transform = 
                {
                    glm::vec3(3+x * 2,  2*y, 3+2*z),
                    glm::vec3(0,0,0),
                    glm::vec3(1,1,1)
                };

                entityCommandInfo.AddComponent<Transform>(transform, transformComponentType);
                commandPool.ScheduleCreationCommand(entity, &entityCommandInfo);
                entityCommandInfo.Clean();
                id++;
            }

        }
    }

    Entity cameraEntity = {id, 0};
    Transform transform = {
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1)
    };

    Camera camera = 
    {
        glm::vec3(0,0,1),
        glm::vec3(0,1,0),
        4,
        0.3f
    };

    entityCommandInfo.AddComponent<Transform>(transform, transformComponentType);
    entityCommandInfo.AddComponent<Camera>(camera, cameraComponentType);
    commandPool.ScheduleCreationCommand(cameraEntity, &entityCommandInfo);

    QueryInfo queryInfo(transformComponentType, true);
    uint32_t queryTicket = EntityManager::RegisterQuery(queryInfo);
    QueryInfo queryInfo2(transformComponentType | cameraComponentType, true);
    uint32_t queryTicket2 = EntityManager::RegisterQuery(queryInfo2);

    EntityManager::ExecuteEntityCommands();
}

void Update(const float deltaTime)
{
    #pragma region Camera
    const bool* keyboard = SDL_GetKeyboardState(NULL);

    Table& cameraTable = EntityManager::GetTablesFromQuery(1)[0];

    std::byte& batch = cameraTable.GetComponentsBatch(0);
    const MemoryInfo cameraMemInfo = cameraTable.GetMemoryInfo(cameraComponentType);
    const MemoryInfo transformMemInfo = cameraTable.GetMemoryInfo(transformComponentType);

    Camera& camera = cameraTable.GetComponent<Camera>(batch, 0, cameraMemInfo);
    Transform& transform = cameraTable.GetComponent<Transform>(batch, 0, transformMemInfo);

    
    float mouseDeltaX = 0;
    float mouseDeltaY = 0;
    SDL_GetRelativeMouseState(&mouseDeltaX, &mouseDeltaY);
    
    camera.yaw += (float)mouseDeltaX * camera.sensitivity;
    camera.pitch = glm::clamp(camera.pitch - (float)mouseDeltaY * camera.sensitivity, -89.0f, 89.0f);
    
    float radYaw = glm::radians(camera.yaw);
    float radPitch = glm::radians(camera.pitch);

    camera.forward.x = glm::cos(radPitch) * glm::sin(radYaw);
    camera.forward.y = glm::sin(radPitch);
    camera.forward.z = glm::cos(radPitch) * glm::cos(radYaw);


    camera.right = glm::normalize(glm::cross(glm::vec3(0,1,0), camera.forward));
    camera.up = glm::normalize(glm::cross(camera.forward, camera.right));

    glm::vec3 movement = glm::vec3(0,0,0);
    if (keyboard[SDL_SCANCODE_W]) movement += camera.forward;
    if (keyboard[SDL_SCANCODE_S]) movement -= camera.forward;
    if (keyboard[SDL_SCANCODE_D]) movement += camera.right;
    if (keyboard[SDL_SCANCODE_A]) movement -= camera.right;
    if (keyboard[SDL_SCANCODE_E]) movement += camera.up;
    if (keyboard[SDL_SCANCODE_Q]) movement -= camera.up;
    
    if (glm::length(movement) > 0.0f)
    {
        movement = glm::normalize(movement);
    }

    if(keyboard[SDL_SCANCODE_LSHIFT])
    {
        transform.Position += movement * deltaTime * camera.speed * 2.5f;
    }
    else 
    {
        transform.Position += movement * deltaTime * camera.speed;
    }
    #pragma endregion
    
    uint64_t tick = static_cast<uint64_t>(SDL_GetTicks());
    Table& table = EntityManager::GetTablesFromQuery(0)[0];
    uint32_t batchesCount = table.GetBatchesCount();
    const MemoryInfo memoryInfo = table.GetMemoryInfo(transformComponentType);
    
    for(uint32_t i = 0; i < batchesCount; i++)
    {
        std::byte& batch = table.GetComponentsBatch(i);
        uint32_t componentsCount = table.GetComponentsCountPerBatch(i);
        Transform& transformArray = table.GetComponentArray<Transform>(batch, memoryInfo);

        for (uint32_t j = 0; j < componentsCount; j++)
        {
            Transform& transform = table.GetComponent<Transform>(transformArray, j, memoryInfo);

            transform.Rotation = glm::vec3(
                accumulator + transform.Position.x,
                accumulator + transform.Position.x * 3 / transform.Position.z,
                accumulator + transform.Position.y / 3
            );
        }
    }

    accumulator += deltaTime;
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);