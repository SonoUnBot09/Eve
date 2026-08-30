#include <eve/input/Input.hpp>
#include "InputManager.hpp"

using namespace Eve::Input;

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