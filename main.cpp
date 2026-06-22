#include "source/Application.hpp"

using namespace std;

int main(int argc, char* argv[])
{

    std::cout <<"Hello World!" <<endl;

    Application app;
    if(app.Initialize())
    {
        std::cout <<"A" <<endl;
        app.Start();
        app.Run();
    }

    app.Shutdown();
    return 0;
}