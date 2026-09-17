# **What is ECS:**

Everything in Eve is based on the ECS which stands for *Entity, Component, System*; Those are the three main pillars of the ECS architecture. 

---

- **Entities** are just a number a 64-bit number which identifies that entity in the world.

- **Components** are the real data that an Entity could have, for example an Entity could be somewhere in space so you attach a Transform *component* to that Entity so it can have position, rotation and scale.

- **Systems** are the code responsible to modify component's data, create and destroy entities.



# **How Does It Work In Eve:**

- Entities and components are managed by the `EntityManager` which keeps track of all the entites and data of the components. Go to `entity_manager.md`.

- Components registration is the act of assign a bit-index of a 64-bit number to a component. To see more go to `components_registry.md`.

- Stages