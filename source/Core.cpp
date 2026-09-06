#include <graphics/GraphicsCore.hpp>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "input/InputManager.hpp"
#include <Core.hpp>
#include <eve/entities/EntityManager.hpp>
#include <entities/systems/SystemDispatcher.hpp>
#include <input/InputManager.hpp>
#include <eve/debug/Debug.hpp>

using namespace Eve::Graphics;
using namespace Eve::Entities;
using namespace Eve::Input;
using namespace Eve::Debug;
using namespace Eve;

namespace 
{
    void UpdateKeys()
    {
        int numKeys = 0;
        const bool* keys = SDL_GetKeyboardState(&numKeys);

        for(uint32_t i = 0; i < numKeys; i++)
        {
            if(!keys[i]) { continue; }

            InputManager::UpdateKeys(static_cast<SDL_Scancode>(i));
        }
    }

    void UpdateMouse()
    {
        float x, y;
        uint32_t mouseState = SDL_GetMouseState(&x, &y);

        uint32_t leftButton = mouseState & SDL_BUTTON_LMASK;
        uint32_t rightButton = mouseState & SDL_BUTTON_RMASK;
        uint32_t wheelButton = mouseState & SDL_BUTTON_MMASK;

        if (leftButton > 0) 
        {
           InputManager::UpdateMouse(leftButton);
        }

        if (rightButton > 0) 
        {
            InputManager::UpdateMouse(rightButton);
        }

        if (wheelButton > 0) 
        {
            InputManager::UpdateMouse(wheelButton);
        }

        float xRel, yRel;
        SDL_GetRelativeMouseState(&xRel, &yRel);

        MouseState state
        {
            .MousePos {x, y},
            .MouseDelta {xRel, yRel}
        };

        InputManager::SetMouseState(state);
    }
}

bool Core::Initialize(std::vector<std::string>& searchShaderPaths)
{
    if(!GraphicsCore::Initialize(searchShaderPaths))
    {
        std::cout << "Eve graphics initialization failed" << std::endl;
        return false;
    }

    EntityManager::Initialize(1000);

    return true;
}

void Core::Start()
{
    SystemDispatcher::ExecuteAwakeStage();
    
    EntityManager::ExecuteAllCommandPools();

    SystemDispatcher::ExecuteStartStage();

    EntityManager::ExecuteAllCommandPools();
}

void Core::Run()
{
    uint32_t fps = 0;
    float timer = 0;
    uint64_t lastTick = SDL_GetTicksNS();
    uint64_t currentTick = 0;
    while(isAppRunning)
    {
        currentTick = SDL_GetTicksNS();

        uint64_t elapsedNS = currentTick - lastTick;

        const float deltaTime = (double)elapsedNS / SDL_NS_PER_SECOND;

        lastTick = currentTick;
        
        InputManager::ResetKeys();

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
            else if(event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == 0)
            {
                InputManager::UpdateKeysDown(event);
            }
            else if(event.type == SDL_EVENT_KEY_UP && event.key.repeat == 0)
            {
                InputManager::UpdateKeysUp(event);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                InputManager::UpdateMouseDown(event);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                InputManager::UpdateMouseUp(event);
            }
            else if(event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
            {
                InputManager::SetWindowGainedFocus();
            }
            else if(event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
            {
                InputManager::SetWindowLostFocus();
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
        
        UpdateKeys();
        UpdateMouse();
        
        if(timer > 1)
        {
            print("FPS: " + std::to_string(fps));
            timer = 0;
            fps = 0;
        }

        timer += deltaTime;
        fps++;

        SystemDispatcher::ExecuteUpdateStage(deltaTime);

        EntityManager::ExecuteAllCommandPools();

        GraphicsCore::Render();
        
        //elapsedFrames++;
    }
}

void Core::Shutdown()
{
    GraphicsCore::Destroy();
}