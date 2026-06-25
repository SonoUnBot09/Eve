#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

namespace Utils
{
    inline std::string readTextFile(std::string path)
    {
        std::ifstream infile(path);
        if(infile.is_open())
        {
            std::stringstream buffer;
            buffer << infile.rdbuf();

            const std::string output = buffer.str();

            infile.close();

            return output;
        }

        return std::string();

    }
}