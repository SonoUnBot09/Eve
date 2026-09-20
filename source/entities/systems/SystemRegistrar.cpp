#include <eve/entities/SystemRegistrar.hpp>
#include "SystemDispatcher.hpp"

using namespace Eve::Entities;

// Awake, Start, Shutdown
SystemRegistrar::SystemRegistrar(std::function<void(uint32_t)> function, SystemStage stage)
{
    switch (stage)
    {
        case SystemStage::Start:
            SystemDispatcher::GetStartStage().push_back(function);
            break;
        case SystemStage::Awake:
            SystemDispatcher::GetAwakeStage().push_back(function);
            break;
        case SystemStage::Shutdown:
            SystemDispatcher::GetShutdownStage().push_back(function);
        default:
            break;
    }
}

// Update
SystemRegistrar::SystemRegistrar(std::function<void(float, uint32_t)> function, SystemStage stage)
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