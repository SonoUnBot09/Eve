#pragma once

#include <bit>
#include <cstddef>
#include <typeindex>
#include <vector>
#include <array>
#include <optional>

#include <eve/entities/Type.hpp>

namespace Eve::Entities
{
    class ComponentsRegistry
    {
        public:

            template<typename T>
            inline static uint32_t RegisterComponent()
            {
                if(bitAndType.capacity() == 0)
                {
                    bitAndType.reserve(64);
                }

                std::type_index componentType = std::type_index(typeid(T));
                std::size_t componentSize = sizeof(T);

                if (GetComponentBit<T>().to_ullong() != 0)
                {
                    return std::countr_zero(GetComponentBit<T>().to_ullong());
                }

                Type type = 0;
                type.set(componentBitIndex);
                bitAndSize[componentBitIndex] = {type, componentSize};

                std::pair<Type, std::type_index> value {type, componentType};
                auto it = std::lower_bound(
                    bitAndType.cbegin(), 
                    bitAndType.cend(), 
                    componentType,
                    [](const std::pair<Type, std::type_index>& element, const std::type_index& val) {
                        return element.second < val;
                    }
                );
                
                bitAndType.insert(it, value);

                uint32_t assignedTicket = componentBitIndex;
                componentBitIndex++;
                
                return assignedTicket;
            }

            template<typename T>
            inline static Type GetComponentBit()
            {
                std::type_index componentType = std::type_index(typeid(T));
                auto it = std::lower_bound
                (
                    bitAndType.cbegin(),
                    bitAndType.cbegin() + componentBitIndex,
                    componentType,
                    [](const std::optional<std::pair<Type, std::type_index>>& element, const std::type_index& value) {
                        return element->second < value;
                    }
                );

                if(it != bitAndType.cend() && it->second == componentType)
                {
                    return it->first;
                }
                
                return 0;
            }
            inline static Type GetComponentBit(uint32_t componentTicket)
            {
                return bitAndSize[componentTicket].first;
            }

            template<typename ... Components>
            inline static Type GetComponentMask()
            {
                return (GetComponentBit<Components>() | ...);
            }

            inline static size_t GetComponentSize(uint32_t componentTicket)
            {
                return bitAndSize[componentTicket].second;
            }
            inline static size_t GetComponentSize(Type componentType)
            {
                uint32_t index = std::countr_zero(componentType.to_ullong());
                return bitAndSize[index].second;
            }

        private:

            inline static uint32_t componentBitIndex = 0;
            inline static std::array<std::pair<Type, size_t>, 64> bitAndSize;
            inline static std::vector<std::pair<Type, std::type_index>> bitAndType;
    };
}