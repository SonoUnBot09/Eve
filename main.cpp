#include <Core.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout <<"Hello World!" << std::endl;
    
    std::vector<std::string> searchShaderPaths
    {
        "../../source/shaders"
    };

    Eve::Core app;
    if(app.Initialize(searchShaderPaths))
    {
        app.Start();
        
        app.Run();
    }

    app.Shutdown();

    return 0;
}