#include "SystemDispatcher.hpp"

using namespace Eve::Entities;

void SystemDispatcher::ExecuteAwakeStage()
{
    std::vector<AwakeStage>& systemsFunc = GetAwakeStage();

    for(AwakeStage &func : systemsFunc)
    {
        func(systemId);

        systemId++;
    }
}

void SystemDispatcher::ExecuteStartStage()
{
    std::vector<StartStage>& systemsFunc = GetStartStage();

    for(StartStage &func : systemsFunc)
    {
        func(systemId);

        systemId++;
    }
}

void SystemDispatcher::ExecuteUpdateStage(const float deltaTime)
{
    std::vector<UpdateStage>& systemsFunc = GetUpdateStage();

    for(UpdateStage &func : systemsFunc)
    {
        func(deltaTime, systemId);

        systemId++;
    }

    // Reset system id
    systemId = 0;
}

std::vector<AwakeStage>& SystemDispatcher::GetAwakeStage()
{
    static std::vector<AwakeStage> awakeStages;

    return awakeStages;
}

std::vector<StartStage>& SystemDispatcher::GetStartStage()
{
    static std::vector<StartStage> startStages;

    return startStages;
}

std::vector<UpdateStage>& SystemDispatcher::GetUpdateStage()
{
    static std::vector<UpdateStage> updateStages;

    return updateStages;
}