# **Systems:**

Systems are based on the concept of stages, in Eve there are four stages: `Awake`, `Start`, `Update`, `Shutdown`.

`Awake`, `Start` and `Shutdown` are called once per system insted `Update` is called each frame for each system that recorded the stage. 

## **How To Record Stages:**

With the file `eve\entities\SystemRegistrar.hpp` you can register system's stages so that Eve will call them.

### **Example:**

    #include<eve/entities/SystemRegistrar.hpp>
    #include <cstdint>

    using namespace Eve::Entities;

    namespace
    {
        void Awake(uint32_t systemId)
        {
            std::cout << "Awake has been called" << std::endl;
        }

        void Start(uint32_t systemId)
        {
            std::cout << "Start has been called" << std::endl;
        }

        void Update(float deltaTime, uint32_t systemId)
        {
            std::cout << "Update has been called" << std::endl;
        }
        
        void Shutdown(uint32_t systemId)
        {
            std::cout << "Shutdown has been called" << std::endl;
        }

        static SystemRegistrar awake(Awake, SystemStage::Awake);
        static SystemRegistrar start(Start, SystemStage::Start);
        static SystemRegistrar update(Update, SystemStage::Update);
        static SystemRegistrar shutdown(Shutdown, SystemStage::Shutdown);
    }

---
Each stage has a parameter called `systemId` which is an identification number to that system. It is used with some methods with the ECS so that the engine can reuse old preallocated memory instead of continous allocation.

### **Example:**

    #include<eve/entities/SystemRegistrar.hpp>
    #include<eve/entities/EntityManager.hpp>
    #include <eve/entities/EntityCommandInfo.hpp>
    #include<eve/components/Transform.hpp>
    #include <cstdint>

    using namespace Eve::Entities;

    namespace
    {
        void Start(uint32_t systemId)
        {
            Transform transform{};

            EntityCommandInfo entityCommandInfo{};

            entityCommandInfo.AddComponent<Transform>(transform);

            EntityManager::ScheduleCreationCommand(&entityCommandInfo, systemId);
        }

        static SystemRegistrar start(Start, SystemStage::Start);
    }

## **Notes:**

- Between `Awake` and `Start` stages the engine does nothing. They are different to allow the user to decide in which order the stages needs to be executed. This is very usefull when you need to register **Components**, to see more go to `components_registry.md`.