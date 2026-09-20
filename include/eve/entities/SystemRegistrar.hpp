#pragma once

#include <cstdint>
#include <functional>

namespace Eve::Entities
{
    enum class SystemStage
    {
        Awake,
        Start,
        Update,
        Shutdown
    };

    class SystemRegistrar
    {
        public:

            // Awake, Start, Shutdown
            SystemRegistrar(std::function<void(uint32_t)> function, SystemStage stage);

            // Update
            SystemRegistrar(std::function<void(float, uint32_t)> function, SystemStage stage);
    };
}