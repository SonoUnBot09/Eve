#pragma once

#include <cstdint>
#include <array>

#include <eve/input/KeyboardKey.hpp>
#include <eve/input/MouseKey.hpp>
#include <eve/input/MouseState.hpp>

#include <SDL3/SDL.h>

namespace Eve::Input
{
    class InputManager
    {
        public:

            // --- Getters ---
            static bool IsKeyDown(KeyboardKey key);
            static bool IsKeyUp(KeyboardKey key);
            static bool IsKey(KeyboardKey key);
            static bool IsAnyKeyDown(KeyboardKey* key);
            static bool IsAnyKeyUp(KeyboardKey* key);

            static bool IsMouseDown(MouseKey key);
            static bool IsMouseUp(MouseKey key);
            static bool IsMouse(MouseKey key);
            static bool IsAnyMouseDown(MouseKey* key);
            static bool IsAnyMouseUp(MouseKey* key);
            inline static MouseState GetMouseState() { return mouseState; }

            inline static bool WindowGainedFocus() { return windowGainedFocus; }
            inline static bool WindowLostFocus() { return windowLostFocus; }

            // --- Setters ---
            static void ResetKeys();
            
            static void UpdateKeysDown(SDL_Event& event);
            static void UpdateKeysUp(SDL_Event& event);
            static void UpdateKeys(SDL_Scancode key);

            static void UpdateMouseDown(SDL_Event& event);
            static void UpdateMouseUp(SDL_Event& event);
            static void UpdateMouse(uint32_t mouseState);
            inline static void SetMouseState(MouseState state) { mouseState = state; }

            inline static void SetWindowGainedFocus() { windowGainedFocus = true; }
            inline static void SetWindowLostFocus() { windowLostFocus = true; }

        private:

            static KeyboardKey FromSDLToKeyboardKey(SDL_Scancode key);
            static MouseKey FromSDLToMouseKey(uint32_t buttonIndex);
            static MouseKey FromSDLToMouseKeyMask(uint32_t buttonIndex);

            static constexpr uint32_t KEYS_NUMBER = 100;
            static constexpr uint32_t MOUSE_NUMBER = 10;

            inline static std::array<bool, KEYS_NUMBER> keysDown;
            inline static std::array<bool, KEYS_NUMBER> keysUp;
            inline static std::array<bool, KEYS_NUMBER> keys;

            inline static std::array<bool, MOUSE_NUMBER> mouseDown;
            inline static std::array<bool, MOUSE_NUMBER> mouseUp;
            inline static std::array<bool, MOUSE_NUMBER> mouse;

            inline static bool windowGainedFocus = false;
            inline static bool windowLostFocus = false;

            inline static MouseState mouseState;
    };
}