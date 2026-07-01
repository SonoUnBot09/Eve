#include <Application.hpp>

using namespace std;

int main(int argc, char* argv[])
{

    std::cout <<"Hello World!" << std::endl;

    
    Application app;
    if(app.Initialize())
    {
        app.Start();
        
        app.Run();
    }

    app.Shutdown();
    return 0;
}