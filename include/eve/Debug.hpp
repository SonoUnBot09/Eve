#pragma once

#include <iostream>
#include <string>

namespace Debug
{
    inline void printError(std::string errorMessage)
    {
        std::cerr << "ERROR: " << errorMessage << std::endl;
    }

    inline void print(std::string message)
    {
        std::cout << message << std::endl;
    }
}