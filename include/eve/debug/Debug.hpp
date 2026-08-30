#pragma once

#include <iostream>
#include <string>

namespace Eve::Debug
{
    inline static void printError(std::string errorMessage)
    {
        std::cerr << "ERROR: " << errorMessage << std::endl;
    }

    inline static void print(std::string message)
    {
        std::cout << message << std::endl;
    }
}