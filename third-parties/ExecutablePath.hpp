#pragma once

#include <filesystem>
#include <string>
#include <whereami.h>

namespace fs = std::filesystem;

inline static fs::path GetExecutableDirectory() {
    // 1. Chiedi la lunghezza del percorso
    int length = wai_getExecutablePath(NULL, 0, NULL);
    if (length < 0) return fs::current_path(); // Fallback

    // 2. Alloca il buffer e ottieni il percorso e l'indice del nome del file
    std::string buffer(length, '\0');
    int dirname_length = 0;
    wai_getExecutablePath(buffer.data(), length, &dirname_length);

    // 3. Restituisci la parte relativa alla sola cartella padre
    return fs::path(buffer.substr(0, dirname_length));
}