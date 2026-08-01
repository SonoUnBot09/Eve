#include <Application.hpp>
#include <graphics/GraphicsCore.hpp>
#include <Eve/entities/SystemDispatcher.hpp>

using namespace Eve::Graphics;
using namespace Eve::Entities;

bool Application::Initialize()
{
    if(!GraphicsCore::Initialize())
    {
        return false;
    }

    //EntityManager::Initialize();
    
    return true;
}

void Application::Start()
{
    //print("Going to execute start stages");
    SystemDispatcher::ExecuteStartStage();
    //print("Start stage executed");
}

void Application::Run()
{
    uint32_t fps = 0;
    float timer = 0;
    Uint64 lastTick = SDL_GetTicksNS();
    Uint64 currentTick = 0;
    while(isAppRunning)
    {
        currentTick = SDL_GetTicksNS();

        Uint64 elapsedNS = currentTick - lastTick;

        const float deltaTime = (double)elapsedNS / SDL_NS_PER_SECOND;

        lastTick = currentTick;

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_QUIT)
            {
                isAppRunning = false;
                break;
            }
            else if(event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                GraphicsCore::Window.Width = event.window.data1;
                GraphicsCore::Window.Height = event.window.data2;
                continue;
            }
        
            /*
            if(event.type == SDL_EVENT_KEY_DOWN)
            {
                if(event.key.scancode == SDL_SCANCODE_ESCAPE)
                {
                    SDL_SetWindowRelativeMouseMode(window, false);
                    continue;
                }
            }

            if(event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
            {
                SDL_SetWindowRelativeMouseMode(window, false);
                continue;
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {   
                    SDL_SetWindowRelativeMouseMode(window, true);
                    continue;
                }
            }*/
        }
        
        if(timer > 1)
        {
            print(std::to_string(fps));
            timer = 0;
            fps = 0;
        }

        timer += deltaTime;
        fps++;

        SystemDispatcher::ExecuteUpdateStage(deltaTime);

        //Render();
        
    }
}

void Application::Shutdown()
{
    //EntityManager::Destroy();
    GraphicsCore::Destroy();
}