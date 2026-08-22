#pragma once

#include <cstdint>
#include <vector>

namespace Eve::Entities
{

    using AwakeStage = void(*)(uint32_t);
    using StartStage = void(*)(uint32_t);
    using UpdateStage = void(*)(float, uint32_t);

    class SystemDispatcher
    {
        public:
        
            static void ExecuteAwakeStage();
            static void ExecuteStartStage();
            static void ExecuteUpdateStage(const float deltaTime);

        private:

            static std::vector<AwakeStage>& GetAwakeStage();

            static std::vector<StartStage>& GetStartStage();

            static std::vector<UpdateStage>& GetUpdateStage();

            inline static uint32_t systemId;

        friend class SystemRegistrar;
    };
}