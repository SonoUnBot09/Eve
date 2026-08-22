#pragma once

#include <cstdint>

namespace Eve::Entities
{
    enum class SystemStage
    {
        Awake,
        Start,
        Update
    };

    class SystemRegistrar
    {
        public:

            // Awake, Start
            SystemRegistrar(void(* function)(uint32_t), SystemStage stage);

            // Update
            SystemRegistrar(void(* function)(float, uint32_t), SystemStage stage);
    };
}