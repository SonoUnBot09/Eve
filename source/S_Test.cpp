#include "Eve/ComponentsRegistry.hpp"
#include <Eve/SystemDispatcher.hpp>
#include <Eve/Debug.hpp>
#include <string>
#include <Eve/EntityManager.hpp>
#include <components/Transform.hpp>

using namespace Eve::Entities;

void Start()
{
    Type componentType = ComponentsRegistry::GetComponentBit<Transform>();
    EntityCommandInfo* entityCommandInfo = new EntityCommandInfo(36, 1);
    EntityCommandPool* commandPool = new EntityCommandPool(0, 0, 0, 36*0, 0);
    uint32_t id = 0;

    for(uint32_t x = 0; x < 5; x++)
    {
        for(uint32_t y = 0; y < 5; y++)
        {
            for (uint32_t z = 0; z < 5; z++)
            {
                Entity entity = {id, 0};
                Transform transform = 
                {
                    glm::vec3(x,y,z),
                    glm::vec3(0,0,0),
                    glm::vec3(1,1,1)
                };
                entityCommandInfo->AddComponent<Transform>(transform, componentType);
                Debug::printError("AAAAAAAAAAAAAAAAAAAAA");
                commandPool->ScheduleCreationCommand(entity, entityCommandInfo);
                Debug::printError("BBBBBBBBBBBBBBBBBBBBBBBB");
                Debug::printError("CCCCCCCCCCCCCCCCCCCCCCC");
                id++;
            }

        }
    }

    EntityManager::RegisterEntityCommandPool(*commandPool);
    EntityManager::ExecuteEntityCommands();

    delete entityCommandInfo;
    delete commandPool;
}

void Update(const float deltaTime)
{
    Debug::print("Update, ms: " + std::to_string(deltaTime));
}

//static SystemRegistrar start(Start, SystemStage::Start);
//static SystemRegistrar update(Update, SystemStage::Update);