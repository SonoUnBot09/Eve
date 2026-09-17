# **Entity Manager:**

The Entity Manager handles **Components** and **Entities**. You can find it in `eve/entities/EntityManager.hpp`. For performance reason the Entity Manager uses something called `Archtype`.

### **What Is An Archtype?**

An `Archtype` is a 64-bit number which combines more `Types` of different components. To see more about `Type` go to `components_registry.md`. Each `Archtype` has a `Table`.

### **What Is A Table?**

A `Table` is a class which contains all the components of that `Archtype`. To know more about `Archtype` and `ECS` architecture click [here](https://docs.unity3d.com/Packages/com.unity.entities@0.2/manual/ecs_core.html). Each `Table` has its own `Batches`.

### **What Is A Batch?**
  
A `Batch` is piece of memory where components are stored, instead of having a large and single block of memory for an `Archtype`, Eve has multiple `Batches` per `Archtype` so that the memory usage is optimized and there are not waste. `Batches` are handle internally by Eve.

## **How To Create A Table?**

To create a `Table` you can use the `CreateTable` method in the `EntityManager`:

    EntityManager::CreateTable(Type archtype, uint32_t batchSizeInBytes);

As the parameters suggest you need to give to the method the `Archtype` and the size in bytes of each `Batch` that will be allocated by the `Table`.

## **How To Get A Table?**

To get a `Table` you can use the `GetTables` method in the `EntityManager`:

    QueryInfo queryInfo
    {
        .ComponentsRequired = myArchtype,
        .IsExclusive = false
    };

    QueryResult queryResult = EntityManager::GetTables(queryInfo);

The `ComponentsRequired` member of queryInfo are the components you are looking for. The `IsExclusive` member checks if you want only the table with that specific components, if it is set to false it will search all the `Table` with **at least** that components you specified, if it is set to true it will search the only `Table` with that components you specified.

The return type is a QueryResult which contains all the `Tables` found with the `QueryInfo` requirements. You can get the number of `Tables` found by calling `GetTablesCount`:

    uint32_t tablesCount = queryResult.GetTablesCount();

Then you can get the `Tables` with the method `GetTable(uint32_t index)`:
    
    Table& table = queryResult.GetTable(0);

It get as a parameter a uint32_t index and returns a reference to a `Table` with its batches, components and entities.

**WARNING:** The index given to the `GetTable` method must be less than `tablesCount`. 

## **Entity Commands:**

To create and destroy `Entities` there two methods in the `EntityManager`:

- `ScheduleCreationCommand(EntityCommandInfo* commandInfo, uint32_t systemId)`
- `ScheduleDestructionCommand(Entity entity, uint32_t systemId)`

---

To create a new Entity you need to use the `ScheduleCreationCommand(EntityCommandInfo* commandInfo, uint32_t systemId)` method. The `EntityCommandInfo` is used to give the `Entity` components, if it is nullptr the `Entity` will be created without any components.

To give the `Entity` some components we can use the `AddComponent(const T& component)` method of `EntityCommandInfo`:

    Transform myTransform{};

    EntityCommandInfo commandInfo{};

    commandInfo.AddComponent<Transform>(myTransform);

    EntityManager::ScheduleCreationCommand(&commandInfo, systemId);

To Destroy an `Entity` we can use the `ScheduleDestructionCommand(Entity entity, uint32_t systemId)` method:

    EntityManager::ScheduleDestructionCommand(entity, systemId);

---

To instead add/remove components to an already existing `Entity` we can use the method:

- `ScheduleTransitionCommand(Entity entity, EntityCommandInfo& commandInfo, uint32_t systemId)`

To add components to the existent `Entity` we can again use the `AddComponent(const T& component)` method of `EntityCommandInfo` and to remove components we can use the `RemoveComponent(Type componentType)` method. In addition when is needed to create a lot of `Entities` and we cant to have high performance and limitate continous allocation we can use the `Clear()` method of `EntityCommandInfo` this clears the memory without deallocating it. Example:

    constexpr uint32_t entitiesCount = 100000;

    EntityCommandInfo entityCommandInfo{};

    for (uint32_t x = 0; x < entitiesCount; x++)
    {
        Transform transform 
        {
            {static_cast<float>(x), 0 , 0},
            glm::quat(1.0, 0.0f, 0.0f, 0.0f),
            {1,1,1}
        };

        entityCommandInfo.AddComponent<Transform>(transform);

        // Using the same EntityCommandInfo
        EntityManager::ScheduleCreationCommand(&entityCommandInfo, systemId);

        // Clears the memory, does not deallocate it
        entityCommandInfo.Clear();
    }

### **Notes:**

- Creating an Entity with an `Archtype` that does not exist will force the `EntityManager` to create a `Table` with that `Archtype` using a default `Batch` size which is set to 16 KB