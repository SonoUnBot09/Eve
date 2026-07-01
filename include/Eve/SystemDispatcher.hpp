#pragma once

#include <vector>

#include <Eve/SystemStage.hpp>

class Application;

namespace Eve::Entities
{
    using StartStage = void(*)();
    using UpdateStage = void(*)(float);

    class SystemDispatcher
    {
        private:    
            static inline std::vector<StartStage>& GetStartStage()
            {
                static std::vector<StartStage> startStages;

                return startStages;
            }

            static inline std::vector<UpdateStage>& GetUpdateStage()
            {
                static std::vector<UpdateStage> updateStages;

                return updateStages;
            }

            static void ExecuteStartStage();
            static void ExecuteUpdateStage(const float deltaTime);

        friend class SystemRegistrar;
        friend class ::Application;
    };

    class SystemRegistrar
    {
        public:

            SystemRegistrar(StartStage function, SystemStage stage)
            {
                SystemDispatcher::GetStartStage().push_back(function);
            }

            SystemRegistrar(UpdateStage function, SystemStage stage)
            {
                SystemDispatcher::GetUpdateStage().push_back(function);
            }
    };
}