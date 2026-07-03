#include <Eve/Entities/SystemDispatcher.hpp>

using namespace Eve::Entities;

void SystemDispatcher::ExecuteStartStage()
{
    std::vector<StartStage>& systemsFunc = GetStartStage();

    for(StartStage &func : systemsFunc)
    {
        func();
    }
}

void SystemDispatcher::ExecuteUpdateStage(const float deltaTime)
{
    std::vector<UpdateStage>& systemsFunc = GetUpdateStage();

    for(UpdateStage &func : systemsFunc)
    {
        func(deltaTime);
    }
}