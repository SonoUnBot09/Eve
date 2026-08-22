#include <Application.hpp>

int main(int argc, char* argv[])
{
    std::cout <<"Hello World!" << std::endl;
    
    Eve::Application app;
    if(app.Initialize())
    {
        app.Start();
        
        app.Run();
    }

    app.Shutdown();
    
    return 0;
}