#pragma once

#include <vector>
#include <eve/entities/SystemStage.hpp>

class Application;

namespace Eve::Entities
{
    using AwakeStage = void(*)(uint32_t);
    using StartStage = void(*)(uint32_t);
    using UpdateStage = void(*)(float, uint32_t);

    class SystemDispatcher
    {
        private:

            static inline std::vector<AwakeStage>& GetAwakeStage()
            {
                static std::vector<AwakeStage> awakeStages;

                return awakeStages;
            }

        
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

            inline static uint32_t systemId;

        friend class SystemRegistrar;
        friend class ::Application;
    };

    class SystemRegistrar
    {
        public:

            SystemRegistrar(void(* function)(uint32_t), SystemStage stage)
            {
                switch (stage)
                {
                    case SystemStage::Start:
                        SystemDispatcher::GetStartStage().push_back(function);
                        break;
                    case SystemStage::Awake:
                        SystemDispatcher::GetAwakeStage().push_back(function);
                        break;
                    default:
                        break;
                }
            }

            SystemRegistrar(void(* function)(float, uint32_t), SystemStage stage)
            {
                switch (stage)
                {
                    case SystemStage::Update:
                        SystemDispatcher::GetUpdateStage().push_back(function);
                        break;
                    default:
                        break;
                }
            }
    };
}