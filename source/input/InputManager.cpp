#include "InputManager.hpp"
#include "eve/input/KeyboardKey.hpp"
#include "eve/input/MouseKey.hpp"

using namespace Eve::Input;

#pragma region Helpers
KeyboardKey InputManager::FromSDLToKeyboardKey(SDL_Scancode key)
{
    switch (key)
    {
        case SDL_SCANCODE_A: return KeyboardKey::KEY_A;
        case SDL_SCANCODE_B: return KeyboardKey::KEY_B;
        case SDL_SCANCODE_C: return KeyboardKey::KEY_C;
        case SDL_SCANCODE_D: return KeyboardKey::KEY_D;
        case SDL_SCANCODE_E: return KeyboardKey::KEY_E;
        case SDL_SCANCODE_F: return KeyboardKey::KEY_F;
        case SDL_SCANCODE_G: return KeyboardKey::KEY_G;
        case SDL_SCANCODE_H: return KeyboardKey::KEY_H;
        case SDL_SCANCODE_I: return KeyboardKey::KEY_I;
        case SDL_SCANCODE_J: return KeyboardKey::KEY_J;
        case SDL_SCANCODE_K: return KeyboardKey::KEY_K;
        case SDL_SCANCODE_L: return KeyboardKey::KEY_L;
        case SDL_SCANCODE_M: return KeyboardKey::KEY_M;
        case SDL_SCANCODE_N: return KeyboardKey::KEY_N;
        case SDL_SCANCODE_O: return KeyboardKey::KEY_O;
        case SDL_SCANCODE_P: return KeyboardKey::KEY_P;
        case SDL_SCANCODE_Q: return KeyboardKey::KEY_Q;
        case SDL_SCANCODE_R: return KeyboardKey::KEY_R;
        case SDL_SCANCODE_S: return KeyboardKey::KEY_S;
        case SDL_SCANCODE_T: return KeyboardKey::KEY_T;
        case SDL_SCANCODE_U: return KeyboardKey::KEY_U;
        case SDL_SCANCODE_V: return KeyboardKey::KEY_V;
        case SDL_SCANCODE_W: return KeyboardKey::KEY_W;
        case SDL_SCANCODE_X: return KeyboardKey::KEY_X;
        case SDL_SCANCODE_Y: return KeyboardKey::KEY_Y;
        case SDL_SCANCODE_Z: return KeyboardKey::KEY_Z;

        case SDL_SCANCODE_0: return KeyboardKey::KEY_0;
        case SDL_SCANCODE_1: return KeyboardKey::KEY_1;
        case SDL_SCANCODE_2: return KeyboardKey::KEY_2;
        case SDL_SCANCODE_3: return KeyboardKey::KEY_3;
        case SDL_SCANCODE_4: return KeyboardKey::KEY_4;
        case SDL_SCANCODE_5: return KeyboardKey::KEY_5;
        case SDL_SCANCODE_6: return KeyboardKey::KEY_6;
        case SDL_SCANCODE_7: return KeyboardKey::KEY_7;
        case SDL_SCANCODE_8: return KeyboardKey::KEY_8;
        case SDL_SCANCODE_9: return KeyboardKey::KEY_9;

        case SDL_SCANCODE_UP:    return KeyboardKey::KEY_UP;
        case SDL_SCANCODE_DOWN:  return KeyboardKey::KEY_DOWN;
        case SDL_SCANCODE_LEFT:  return KeyboardKey::KEY_LEFT;
        case SDL_SCANCODE_RIGHT: return KeyboardKey::KEY_RIGHT;

        case SDL_SCANCODE_RETURN:    return KeyboardKey::KEY_ENTER;
        case SDL_SCANCODE_ESCAPE:    return KeyboardKey::KEY_ESCAPE;
        case SDL_SCANCODE_SPACE:     return KeyboardKey::KEY_SPACE;
        case SDL_SCANCODE_BACKSPACE: return KeyboardKey::KEY_BACKSPACE;
        case SDL_SCANCODE_TAB:       return KeyboardKey::KEY_TAB;
        case SDL_SCANCODE_CAPSLOCK:  return KeyboardKey::KEY_CAPSLOCK;

        case SDL_SCANCODE_LSHIFT: return KeyboardKey::KEY_LSHIFT;
        case SDL_SCANCODE_RSHIFT: return KeyboardKey::KEY_RSHIFT;
        case SDL_SCANCODE_LCTRL:  return KeyboardKey::KEY_LCTRL;
        case SDL_SCANCODE_RCTRL:  return KeyboardKey::KEY_RCTRL;
        case SDL_SCANCODE_LALT:   return KeyboardKey::KEY_LALT;
        case SDL_SCANCODE_RALT:   return KeyboardKey::KEY_RALT;

        case SDL_SCANCODE_F1:  return KeyboardKey::KEY_F1;
        case SDL_SCANCODE_F2:  return KeyboardKey::KEY_F2;
        case SDL_SCANCODE_F3:  return KeyboardKey::KEY_F3;
        case SDL_SCANCODE_F4:  return KeyboardKey::KEY_F4;
        case SDL_SCANCODE_F5:  return KeyboardKey::KEY_F5;
        case SDL_SCANCODE_F6:  return KeyboardKey::KEY_F6;
        case SDL_SCANCODE_F7:  return KeyboardKey::KEY_F7;
        case SDL_SCANCODE_F8:  return KeyboardKey::KEY_F8;
        case SDL_SCANCODE_F9:  return KeyboardKey::KEY_F9;
        case SDL_SCANCODE_F10: return KeyboardKey::KEY_F10;
        case SDL_SCANCODE_F11: return KeyboardKey::KEY_F11;
        case SDL_SCANCODE_F12: return KeyboardKey::KEY_F12;

        case SDL_SCANCODE_INSERT:     return KeyboardKey::KEY_INSERT;
        case SDL_SCANCODE_DELETE:     return KeyboardKey::KEY_DELETE;
        case SDL_SCANCODE_HOME:       return KeyboardKey::KEY_HOME;
        case SDL_SCANCODE_END:        return KeyboardKey::KEY_END;
        case SDL_SCANCODE_PAGEUP:     return KeyboardKey::KEY_PAGEUP;
        case SDL_SCANCODE_PAGEDOWN:   return KeyboardKey::KEY_PAGEDOWN;
        case SDL_SCANCODE_PRINTSCREEN: return KeyboardKey::KEY_PRINTSCREEN;
        case SDL_SCANCODE_PAUSE:      return KeyboardKey::KEY_PAUSE;

        case SDL_SCANCODE_KP_0: return KeyboardKey::KEY_KP_0;
        case SDL_SCANCODE_KP_1: return KeyboardKey::KEY_KP_1;
        case SDL_SCANCODE_KP_2: return KeyboardKey::KEY_KP_2;
        case SDL_SCANCODE_KP_3: return KeyboardKey::KEY_KP_3;
        case SDL_SCANCODE_KP_4: return KeyboardKey::KEY_KP_4;
        case SDL_SCANCODE_KP_5: return KeyboardKey::KEY_KP_5;
        case SDL_SCANCODE_KP_6: return KeyboardKey::KEY_KP_6;
        case SDL_SCANCODE_KP_7: return KeyboardKey::KEY_KP_7;
        case SDL_SCANCODE_KP_8: return KeyboardKey::KEY_KP_8;
        case SDL_SCANCODE_KP_9: return KeyboardKey::KEY_KP_9;

        case SDL_SCANCODE_KP_ENTER: return KeyboardKey::KEY_KP_ENTER;
        case SDL_SCANCODE_KP_PLUS:  return KeyboardKey::KEY_KP_PLUS;
        case SDL_SCANCODE_KP_MINUS: return KeyboardKey::KEY_KP_MINUS;

        default:    
            // Unknown key
            return static_cast<KeyboardKey>(0);
    }
}
MouseKey InputManager::FromSDLToMouseKey(uint32_t buttonIndex)
{
    switch (buttonIndex) 
    {
        case (SDL_BUTTON_LEFT) : return MouseKey::BUTTON_LEFT;
        case (SDL_BUTTON_RIGHT) : return MouseKey::BUTTON_RIGHT;
        case (SDL_BUTTON_MIDDLE) : return MouseKey::WHEEL;
        default:
            return static_cast<MouseKey>(0);
    };
}
MouseKey InputManager::FromSDLToMouseKeyMask(uint32_t buttonIndex)
{
    switch (buttonIndex) 
    {
        case (SDL_BUTTON_LMASK) : return MouseKey::BUTTON_LEFT;
        case (SDL_BUTTON_RMASK) : return MouseKey::BUTTON_RIGHT;
        case (SDL_BUTTON_MMASK) : return MouseKey::WHEEL;
        default:
            return static_cast<MouseKey>(0);
    };
}
#pragma endregion

void InputManager::ResetKeys()
{
    keysDown.fill(false);
    keysUp.fill(false);
    keys.fill(false);
    
    mouseDown.fill(false);
    mouseUp.fill(false);
    mouse.fill(false);

    windowGainedFocus = false;
    windowLostFocus = false;
}


void InputManager::UpdateKeysDown(SDL_Event& event)
{
    KeyboardKey keycode = FromSDLToKeyboardKey(event.key.scancode);

    uint32_t index = static_cast<uint32_t>(keycode);

    if(index == 0) { return; }

    keysDown[index] = true;
}

void InputManager::UpdateKeysUp(SDL_Event& event)
{
    KeyboardKey keycode = FromSDLToKeyboardKey(event.key.scancode);

    uint32_t index = static_cast<uint32_t>(keycode);

    if(index == 0) { return; }

    keysUp[index] = true;
}

void InputManager::UpdateKeys(SDL_Scancode key)
{
    KeyboardKey keycode = FromSDLToKeyboardKey(key);

    uint32_t index = static_cast<uint32_t>(keycode);

    if(index == 0) { return; }

    keys[index] = true;
}

bool InputManager::IsAnyKeyDown(KeyboardKey* key)
{
    bool found = false;
    for(uint32_t i = 0; i < KEYS_NUMBER; i++)
    {
        if(keysDown[i])
        {
            found = true;
            if(key != nullptr)
            {
                *key = static_cast<KeyboardKey>(i);
            }
            break;
        }
    }

    return found;
}

bool InputManager::IsAnyKeyUp(KeyboardKey* key)
{
    bool found = false;
    for(uint32_t i = 0; i < KEYS_NUMBER; i++)
    {
        if(keysUp[i])
        {
            found = true;
            if(key != nullptr)
            {
                *key = static_cast<KeyboardKey>(i);
            }
            break;
        }
    }

    return found;
}


void InputManager::UpdateMouseDown(SDL_Event& event)
{
    MouseKey key = FromSDLToMouseKey(event.button.button);

    uint32_t index = static_cast<uint32_t>(key);

    if(index == 0) { return; }

    mouseDown[index] = true;
}

void InputManager::UpdateMouseUp(SDL_Event& event)
{
    MouseKey key = FromSDLToMouseKey(event.button.button);

    uint32_t index = static_cast<uint32_t>(key);

    if(index == 0) { return; }

    mouseUp[index] = true;
}

void InputManager::UpdateMouse(uint32_t mouseState)
{
    MouseKey key = FromSDLToMouseKeyMask(mouseState);

    uint32_t index = static_cast<uint32_t>(key);

    if(index == 0) { return; }

    mouse[index] = true;
}

bool InputManager::IsAnyMouseDown(MouseKey* key)
{
    bool found = false;
    for(uint32_t i = 0; i < MOUSE_NUMBER; i++)
    {
        if(mouseDown[i])
        {
            found = true;
            if(key != nullptr)
            {
                *key = static_cast<MouseKey>(i);
            }
            break;
        }
    }

    return found;
}

bool InputManager::IsAnyMouseUp(MouseKey* key)
{
    bool found = false;
    for(uint32_t i = 0; i < MOUSE_NUMBER; i++)
    {
        if(mouseUp[i])
        {
            found = true;
            if(key != nullptr)
            {
                *key = static_cast<MouseKey>(i);
            }
            break;
        }
    }

    return found;
}

#pragma region Getters

bool InputManager::IsKeyDown(KeyboardKey key)
{
    return keysDown[static_cast<uint32_t>(key)];
}

bool InputManager::IsKeyUp(KeyboardKey key)
{
    return keysUp[static_cast<uint32_t>(key)];
}

bool InputManager::IsKey(KeyboardKey key)
{
    return keys[static_cast<uint32_t>(key)];
}

bool InputManager::IsMouseDown(MouseKey key)
{
    return mouseDown[static_cast<uint32_t>(key)];
}

bool InputManager::IsMouseUp(MouseKey key)
{
    return mouseUp[static_cast<uint32_t>(key)];
}

bool InputManager::IsMouse(MouseKey key)
{
    return mouse[static_cast<uint32_t>(key)];
}

#pragma endregion