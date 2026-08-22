#pragma once

#include <eve/entities/Type.hpp>

namespace Eve::Entities
{
    struct QueryInfo
    {
        Type ComponentsRequired;
        bool IsExclusive;

        QueryInfo() = default;
        QueryInfo(Type componentsRequired, bool isExclusive) :
        ComponentsRequired(componentsRequired),
        IsExclusive(isExclusive) {};

        bool operator==(const QueryInfo& other) const {
            return 
                this->ComponentsRequired == other.ComponentsRequired &&
                this->IsExclusive == other.IsExclusive; 
        }
    };
}

inline void hash_combine(std::size_t& seed) noexcept {}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) noexcept {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

template <>
struct std::hash<Eve::Entities::QueryInfo> {
    std::size_t operator()(const Eve::Entities::QueryInfo& q) const noexcept {
        std::size_t seed = 0;
        hash_combine(seed, q.ComponentsRequired, q.IsExclusive);
        return seed;
    }
};