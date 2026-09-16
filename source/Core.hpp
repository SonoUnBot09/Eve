#pragma once

#include <eve/Eve.hpp>

namespace Eve
{
    class Core
    {
        public:

            static bool Initialize(EveEngineCreateInfo info);
            static void Run();
            static void Shutdown();

        private:

            static void SetEveSetting(EveEngineCreateInfo& info);
            inline static bool isAppRunning = true;
    };
}