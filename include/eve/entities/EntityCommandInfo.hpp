#pragma once

#include <eve/entities/ComponentsRegistry.hpp>
#include <cstdint>
#include <vector>

#include <eve/entities/Type.hpp>

namespace Eve::Entities
{
    struct EntityCommandInfo
    {
        public:

            EntityCommandInfo()
            {
                creationComponents.resize(256);
                creationComponentsType.reserve(64);
            }
            EntityCommandInfo(
                uint32_t creationComponentsInitialSize
            )
            {
                creationComponents.resize(creationComponentsInitialSize);
                creationComponentsType.reserve(64);
            }

            template<typename T>
            inline void AddComponent(const T& component)
            {
                Type componentType = ComponentsRegistry::GetComponentBit<T>();
                
                uint32_t size = sizeof(component);
                uint32_t availableSpace = creationComponents.size() - creationComponentsOffset;
                if(availableSpace < size)
                {
                    uint32_t baseSpace = creationComponents.size();
                    creationComponents.resize(baseSpace + 64);
                }

                std::byte* dst = creationComponents.data() + creationComponentsOffset;
                std::memcpy(dst, &component, size);
                creationComponentsType.push_back(componentType);

                creationComponentsOffset += size;
                createComponentsArchtype = (createComponentsArchtype | componentType);

            }
            inline void RemoveComponent(const Type componentType) { destroyComponentsArchtype = (destroyComponentsArchtype | componentType); }

            inline void Clear() 
            { 
                creationComponentsOffset = 0;

                createComponentsArchtype = 0;
                destroyComponentsArchtype = 0;

                creationComponents.clear();
                creationComponentsType.clear();
            }

        private:

            inline const Type GetCreateComponentsArchtype() { return createComponentsArchtype; }
            inline const Type GetDestroyComponentsArchtype() { return destroyComponentsArchtype; }
            inline const size_t GetComponentsArraySize() { return creationComponentsOffset; }

            inline const std::vector<std::byte>& GetCreationComponents() { return creationComponents; }
            inline const std::vector<Type>& GetCreationComponentsType() { return creationComponentsType; }

            uint32_t creationComponentsOffset = 0;

            Type createComponentsArchtype = 0;
            Type destroyComponentsArchtype = 0;
            std::vector<std::byte> creationComponents;
            std::vector<Type> creationComponentsType;

            friend class EntityCommandPool;
    };
}