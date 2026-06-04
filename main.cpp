#include "iostream"
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>

using namespace std;

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init fallita: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    std::cout << "SDL inizializzata correttamente" << std::endl;

    SDL_Quit();
    return 0;
}