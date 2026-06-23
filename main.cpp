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
        std::cout <<"D" <<endl;
        app.Run();
    }

    

    app.Shutdown();
    return 0;
}