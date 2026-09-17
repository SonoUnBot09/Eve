# **Components:**

Components in Eve are identified as a 64-bit number, each bit corresponds to a components. To represent this you can use a type called `Type` in `eve/entities/Type.hpp`, this is an alias for `std::bitset<64>`.

## **How To Register A Component:**

The class responsible to components registration is the `ComponentsRegistry` in `eve/entities/ComponentsRegistry.hpp`.

### **Example:**

    #include<eve/entities/ComponentsRegistry.hpp>
    #include<eve/entities/SystemRegistrar.hpp>
    #include<eve/entities/Type.hpp>
    #include<eve/components/Transform.hpp>

    using namespace Eve::Entities;

    namespace
    {
        void Awake(uint32_t systemId)
        {
            // Register the Transform component
            ComponentsRegistry::RegisterComponent<Transform>();
        }

        static SystemRegistrar awake(Awake, SystemStage::Awake);
    }

To see more about Systems and Stages go to `systems.md`

## **Masks & Combining Types:**

We can get the component sizes and `Type` from their kind and tickets

    // Register the Transform component
    uint32_t componentTicket = ComponentsRegistry::RegisterComponent<Transform>();

    // Get the Transform component type
    Type componentType = ComponentsRegistry::GetComponentBit<Transform>();
    Type componentTypeByTicket = ComponentsRegistry::GetComponentBit(componentTicket);

    // Get the Transform component size
    size_t componentSize = ComponentsRegistry::GetComponentSize(componentType);
    size_t componentSizeByTicket = ComponentsRegistry::GetComponentSize(componentTypeByTicket);

We can combine `Types` and create `Archtypes` that can be used when we search `Tables` with specific `Archtypes` or more general ones.

    // Register the Transform and MyCustomData components
    ComponentsRegistry::RegisterComponent<Transform>();
    ComponentsRegistry::RegisterComponent<MyCustomData>();

    // Get each individual type
    Type transformComponentType = ComponentsRegistry::GetComponentBit<Transform>();
    Type myCustomComponentType = ComponentsRegistry::GetComponentBit<MyCustomData>();

    // Create manually an archtype using the OR operator between two components type
    Type archtype = transformComponentType | myCustomComponentType;

The `ComponentsRegistry` has a method to automate this process:

    // Register the Transform and MyCustomData components
    ComponentsRegistry::RegisterComponent<Transform>();
    ComponentsRegistry::RegisterComponent<MyCustomData>();

    // Create the archtype with the GetComponentMask method in ComponentsRegistry
    Type archtype = ComponentsRegistry::GetComponentMask<Transform, MyCustomData>();

### **Notes:**

- Is reccomended to register components in the `Awake` stage.

## **WARNING:**
- Any usage of unregistered components is **UB**.
- Every component must be POD otherwise the program will crash.