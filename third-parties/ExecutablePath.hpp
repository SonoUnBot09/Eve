#pragma once

#include <filesystem>
#include <string>
extern "C" 
{ 
    #include "where_am_i/whereami.h"
}

namespace fs = std::filesystem;

inline static fs::path GetExecutableDirectory() 
{
    int length = wai_getExecutablePath(NULL, 0, NULL);
    if (length < 0) return fs::current_path();

    std::string buffer(length, '\0');
    int dirname_length = 0;
    wai_getExecutablePath(buffer.data(), length, &dirname_length);

    return fs::path(buffer.substr(0, dirname_length));
}