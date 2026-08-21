#pragma once

#include <cstdint>
#include <vector>

#include <eve/entities/Type.hpp>

namespace Eve::Entities
{
    struct EntityCommandInfo
    {

        EntityCommandInfo()
        {
            creationComponents.resize(2048);
            creationComponentsType.reserve(64);
        }
        EntityCommandInfo(
            uint32_t creationComponentsInitialSize, 
            uint32_t creationComponentsTypeInitialSize
        )
        {
            creationComponents.resize(creationComponentsInitialSize);
            creationComponentsType.reserve(creationComponentsTypeInitialSize);
        }

        public:

            template<typename T>
            inline void AddComponent(const T& component, const Type componentType)
            {
                uint32_t size = sizeof(component);
                uint32_t availableSpace = creationComponents.size() - creationComponentsOffset;
                if(availableSpace < size)
                {
                    uint32_t baseSpace = creationComponents.size();
                    creationComponents.resize(baseSpace + 512);
                }

                std::byte* dst = creationComponents.data() + creationComponentsOffset;
                std::memcpy(dst, &component, size);
                creationComponentsType.push_back(componentType);

                creationComponentsOffset += size;
                createComponentsArchtype = (createComponentsArchtype | componentType);

            }
            inline void RemoveComponent(const Type componentType) { destroyComponentsArchtype = (destroyComponentsArchtype | componentType); }

            inline const Type GetCreateComponentsArchtype() { return createComponentsArchtype; }
            inline const Type GetDestroyComponentsArchtype() { return destroyComponentsArchtype; }
            inline const size_t GetComponentsArraySize() { return creationComponentsOffset; }
            inline void Clean() 
            { 
                creationComponentsOffset = 0;

                createComponentsArchtype = 0;
                destroyComponentsArchtype = 0;

                creationComponents.clear();
                creationComponentsType.clear();
            }

        private:

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