#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vector>
#include <array>
#include <string>

#include <Eve/Debug.hpp>
#include <Eve/Utils.hpp>

using namespace Debug;
using namespace Utils;

class Application
{
    public:

        bool Initialize();
        void Start();
        void Run();
        void Shutdown();

    private:

        bool isAppRunning = true;

};