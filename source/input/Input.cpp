#include <eve/input/Input.hpp>
#include "InputManager.hpp"
#include <SDL3/SDL.h>
#include <graphics/GraphicsCore.hpp>

using namespace Eve::Input;
using namespace Eve::Graphics;

bool Input::IsKeyDown(KeyboardKey key)
{
    return InputManager::IsKeyDown(key);
}

bool Input::IsKeyUp(KeyboardKey key)
{
    return InputManager::IsKeyUp(key);
}

bool Input::IsKey(KeyboardKey key)
{
    return InputManager::IsKey(key);
}

bool Input::IsAnyKeyDown(KeyboardKey* key)
{
    return InputManager::IsAnyKeyDown(key);
}

bool Input::IsAnyKeyUp(KeyboardKey* key)
{
    return InputManager::IsAnyKeyUp(key);
}

bool Input::IsMouseDown(MouseKey key)
{
    return InputManager::IsMouseDown(key);
}

bool Input::IsMouseUp(MouseKey key)
{
    return InputManager::IsMouseUp(key);
}

bool Input::IsMouse(MouseKey key)
{
    return InputManager::IsMouse(key);
}

bool Input::IsAnyMouseDown(MouseKey* key)
{
    return InputManager::IsAnyMouseDown(key);
}

bool Input::IsAnyMouseUp(MouseKey* key)
{
    return InputManager::IsAnyMouseUp(key);
}

MouseState Input::GetMouseState()
{
    return InputManager::GetMouseState();
}

void Input::LockMouseAtCenter(bool value)
{
    SDL_SetWindowRelativeMouseMode(GraphicsCore::Window.Window, value);
}

bool Input::WindowGainedFocus()
{
    return InputManager::WindowGainedFocus();
}

bool WindowLostFocus()
{
    return InputManager::WindowLostFocus();
}