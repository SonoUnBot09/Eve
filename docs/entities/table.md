# **Table**

A `Table` is the class responsible to store entities's components and manage batches.

## **Methods:**

- `GetEntitiesCount()`
- `GetComponent(uint32_t index, Type componentType)`
- `GetEntity(uint32_t index)`
- `GetSlotInfo(uint32_t index)`
- `GetComponent(SlotInfo slotInfo, Type componentType)`
- `GetEntity(SlotInfo slotInfo)`

---

    // Return the number of entities in the table
    uint32_t entitiesCount = table.GetEntitiesCount();

The difference between the `GetComponent(uint32_t index, Type componentType)` and `GetComponent(SlotInfo slotInfo, Type componentType)` or between `GetEntity(uint32_t index)` and `GetEntity(SlotInfo slotInfo)` is purely of speed. Both of them give you a component/entity to that index but the varian with SlotInfo does less calculation when you need to iterate over more than a single component. Since components are organized in `Batches` each time `GetComponent(uint32_t index, Type componentType)` or `GetEntity(uint32_t index)` are called, internally they divide the index parameter as two values, the `Batch` index and the local component index inside that `Batch`. 

If this is done for only a single component for each `Entity` this is fine. Example:

    uint32_t entitiesCount = table.GetEntitiesCount();

    Type componentType = ComponentRegistry::GetComponentBit<Transform>();

    for(uint32_t i = 0; i < entitiesCount; i++)
    {
        Transform& transform = table.GetComponent<Transform>(i, componentType);

        // Other operations...
    }

But, when you iterate over more than a single component for each `Entity` this calculation is repeated each time you get a different component. Example:

    uint32_t entitiesCount = table.GetEntitiesCount();

    Type transformType = ComponentRegistry::GetComponentBit<Transform>();
    Type customType = ComponentRegistry::GetComponentBit<CustomData>();

    for(uint32_t i = 0; i < entitiesCount; i++)
    {
        Transform& transform = table.GetComponent<Transform>(i, transformType);

        CustomData& customData = table.GetComponent<CustomData>(i, customType);

        // Other operations...
    }

`SlotInfo` solves this problem eliminating useless calculation:

    uint32_t entitiesCount = table.GetEntitiesCount();

    Type transformType = ComponentRegistry::GetComponentBit<Transform>();
    Type customType = ComponentRegistry::GetComponentBit<CustomData>();

    for(uint32_t i = 0; i < entitiesCount; i++)
    {
        SlotInfo slotInfo = table.GetSlotInfo(i);

        Transform& transform = table.GetComponent<Transform>(slotInfo, transformType);

        CustomData& customData = table.GetComponent<CustomData>(slotInfo, customType);

        Entity& entity = table.GetEntity(slotInfo);

        // Other operations...
    }
---

**WARNING:** The index given to the `GetComponent(...)`, `GetEntity(..)` and `GetSlotInfo(...)` method must be less than `entitiesCount`.