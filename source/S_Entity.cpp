#include "eve/entities/ComponentsRegistry.hpp"
#include "eve/input/KeyboardKey.hpp"
#include "eve/input/MouseKey.hpp"
#include <eve/entities/EntityCommandInfo.hpp>
#include <cstdint>
#include <glm/gtc/quaternion.hpp>
#include <eve/entities/SystemRegistrar.hpp>
#include <eve/components/Transform.hpp>
#include <eve/components/Camera.hpp>
#include <eve/entities/EntityManager.hpp>
#include <eve/input/Input.hpp>
#include <glm/glm.hpp>

using namespace Eve::Entities;
using namespace Eve::Input;

namespace 
{
    static constexpr uint32_t entitiesCount = 4;
    static glm::vec2 previusMousePos = glm::vec2(0,0);

    void Start(uint32_t systemId)
    {
        ComponentsRegistry::RegisterComponent<Transform>();
        ComponentsRegistry::RegisterComponent<Camera>();

        EntityCommandInfo entityCommandInfo{};

        for (uint32_t x = 0; x < entitiesCount; x++)
        {
            for(uint32_t y = 0; y < entitiesCount; y++)
            {
                for (uint32_t z = 0; z < entitiesCount; z++)
                {
                    Transform transform 
                    {
                        {static_cast<float>(x),static_cast<float>(y),static_cast<float>(z)},
                        glm::quat(1.0, 0.0f, 0.0f, 0.0f),
                        {1,1,1}
                    };

                    entityCommandInfo.AddComponent<Transform>(transform);

                    EntityManager::ScheduleCreationCommand(&entityCommandInfo, systemId);

                    entityCommandInfo.Clear();
                }
            }
        }

        Transform cameraTransform
        {
            {0,0,0},
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            {1,1,1}
        };

        Camera camera {{0,0,1}, {0,1,0}, 1, 0.5};

        entityCommandInfo.AddComponent(cameraTransform);
        entityCommandInfo.AddComponent(camera);

        EntityManager::ScheduleCreationCommand(&entityCommandInfo, systemId);
    }

    void UpdateCameraMouseInput(Camera& camera, float mouseDeltaX, float mouseDeltaY, const glm::vec3& worldUp = glm::vec3(0, 1, 0))
    {
        camera.yaw   += mouseDeltaX * camera.sensitivity;
        camera.pitch -= mouseDeltaY * camera.sensitivity;

        if (camera.pitch > 89.0f)  camera.pitch = 89.0f;
        if (camera.pitch < -89.0f) camera.pitch = -89.0f;

        float yawRad   = glm::radians(camera.yaw);
        float pitchRad = glm::radians(camera.pitch);

        glm::vec3 newForward;
        newForward.x = std::sin(yawRad) * std::cos(pitchRad);
        newForward.y = std::sin(pitchRad);
        newForward.z = std::cos(yawRad) * std::cos(pitchRad);

        camera.forward = glm::normalize(newForward);

        camera.right = glm::normalize(glm::cross(worldUp, camera.forward));
        camera.up    = glm::normalize(glm::cross(camera.forward, camera.right));
    }

    void Update(float deltaTime, uint32_t systemId)
    {
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

        Transform& transform = table.GetComponent<Transform>(0, transformComponentType);
        Camera& camera = table.GetComponent<Camera>(0, cameraComponentType);

        MouseState mouseState = Input::GetMouseState();

        
        if(Input::IsMouseDown(MouseKey::BUTTON_LEFT))
        {
            Input::LockMouseAtCenter(true);
        }
        else if (Input::IsKeyDown(KeyboardKey::KEY_ESCAPE)) 
        {
            Input::LockMouseAtCenter(false);
        }

        glm::vec2 mouseDelta = mouseState.MousePos - previusMousePos;
        UpdateCameraMouseInput(camera, mouseDelta.x, -mouseDelta.y);

        transform.Rotation = glm::quat(glm::radians(glm::vec3(camera.pitch, camera.yaw, 0.0f)));

        glm::vec3 moveDir(0.0f, 0.0f, 0.0f);

        if (Input::IsKey(KeyboardKey::KEY_W)) moveDir += camera.forward;
        if (Input::IsKey(KeyboardKey::KEY_S)) moveDir -= camera.forward;

        if (Input::IsKey(KeyboardKey::KEY_D)) moveDir += camera.right;
        if (Input::IsKey(KeyboardKey::KEY_A)) moveDir -= camera.right;

        if (Input::IsKey(KeyboardKey::KEY_E))  moveDir += glm::vec3(0,1,0);
        if (Input::IsKey(KeyboardKey::KEY_Q)) moveDir -= glm::vec3(0,1,0);

        // Boost di velocità con CTRL
        float currentSpeed = camera.speed;
        if (Input::IsKey(KeyboardKey::KEY_LSHIFT)) 
        {
            currentSpeed *= 2.5f;
        }

        transform.Position += moveDir * currentSpeed * deltaTime;

        camera.renderView.SetTRS(transform);

        previusMousePos = mouseState.MousePos;
    }
}

static SystemRegistrar start(Start, SystemStage::Start);
static SystemRegistrar update(Update, SystemStage::Update);