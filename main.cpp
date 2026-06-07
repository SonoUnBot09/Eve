#include "Debug.hpp"
#include "source/Application.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    std::cout <<"Hello World!" <<endl;

    Application app;
    if(!app.Initialize())
    {
        printError("Could not initialize the application");
    }

    app.Shutdown();
    return 0;
}