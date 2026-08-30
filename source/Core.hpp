
#include <cstdint>
#include <vector>
#include <string>
#include "EveSettings.hpp"

namespace Eve
{
    class Core
    {
        public:

            static bool Initialize(std::vector<std::string>& searchShaderPaths);
            static void Start();
            static void Run();
            static void Shutdown();

        private:

            inline static bool isAppRunning = true;
            inline static uint64_t elapsedFrames = Eve::Settings::MAX_FRAMES_IN_FLIGHT;

    };
}