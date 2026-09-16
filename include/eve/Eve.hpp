#pragma once

#include <vector>
#include <string>

namespace Eve
{
    struct EveEngineCreateInfo
    {
        std::vector<std::string> ShaderSearchPaths;
        bool VSync = true;
        bool WindowFullScreen = false;
        bool WindowResizable = true;
        bool WindowBordered = true;
        uint32_t WindowWidth = 620;
        uint32_t WindowHeight = 480;
    };

    class EveEngine
    {
        public:
            static bool Initialize(EveEngineCreateInfo info);
            static void Run();
            static void Shutdown();
    };
}