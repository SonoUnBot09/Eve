#include "EveSettings.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <eve/Debug.hpp>
#include <eve/Utils.hpp>

using namespace Debug;
using namespace Utils;

namespace Eve
{
    class Application
    {
        public:

            static bool Initialize();
            static void Start();
            static void Run();
            static void Shutdown();

        private:

            inline static bool isAppRunning = true;
            inline static uint64_t elapsedFrames = Eve::Settings::MAX_FRAMES_IN_FLIGHT;

    };
}