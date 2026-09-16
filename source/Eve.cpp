#include <eve/Eve.hpp>
#include <Core.hpp>

using namespace Eve;

bool EveEngine::Initialize(EveEngineCreateInfo info)
{
    return Core::Initialize(info);
}

void EveEngine::Run()
{
    Core::Run();
}

void EveEngine::Shutdown()
{
    Core::Shutdown();
}