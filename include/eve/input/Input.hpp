#pragma once

#include <eve/input/KeyboardKey.hpp>
#include <eve/input/MouseKey.hpp>
#include <eve/input/MouseState.hpp>

namespace Eve::Input
{
    class Input
    {
        public:

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

            static MouseState GetMouseState();

            static void LockMouseAtCenter(bool value);
    };
}