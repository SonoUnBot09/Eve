#include <eve/Eve.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    
    Eve::EveEngineCreateInfo info
    {
        .ShaderSearchPaths{"shaders"},
        .VulkanValidationLayers = false,
        .VSync = false,
        .WindowWidth = 740,
        .WindowHeight = 512,
        .WindowTitle = "Eve"
    };
    
    if(Eve::EveEngine::Initialize(info))
    {
        Eve::EveEngine::Run();
    }

    Eve::EveEngine::Shutdown();

    return 0;
}