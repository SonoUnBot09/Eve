#pragma once

#include <cstddef>
#include <typeindex>
#include <unordered_map>

#include <Eve/Type.hpp>

class ComponentsRegistry
{
    public:

        template<typename T>
        inline static void RegisterComponent(Type componentId)
        {
            std::type_index componentType = std::type_index(typeid(T));
            std::size_t componentSize = sizeof(T);

            fromTypeToBit[componentType] = componentId;
            fromBitToSize[componentId] = componentSize;
        }

        template<typename T>
        inline static Type GetComponentBit()
        {
            std::type_index componentType = std::type_index(typeid(T));

            return fromTypeToBit[componentType];
        }

        inline static size_t GetComponentSizeFromBit(Type componentId)
        {
            return fromBitToSize[componentId];
        }

    private:

        inline static std::unordered_map<std::type_index, Type> fromTypeToBit;
        inline static std::unordered_map<Type, size_t> fromBitToSize;
};