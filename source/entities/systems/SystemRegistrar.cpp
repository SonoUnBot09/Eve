#include <eve/entities/SystemRegistrar.hpp>
#include "SystemDispatcher.hpp"

using namespace Eve::Entities;

// Awake, Start
SystemRegistrar::SystemRegistrar(void(* function)(uint32_t), SystemStage stage)
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

// Update
SystemRegistrar::SystemRegistrar(void(* function)(float, uint32_t), SystemStage stage)
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