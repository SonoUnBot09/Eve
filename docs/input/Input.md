# **Inputs:**

Inputs can be intercepted by calling the methods in file `include\input\Input.hpp`.

## **Methods:**

- `IsKeyDown(KeyboardKey key)`
- `IsKeyUp(KeyboardKey key)`
- `IsKey(KeyboardKey key)`
- `IsAnyKeyDown(KeyboardKey* key)`
- `IsAnyKeyUp(KeyboardKey* key)`
- `IsMouseDown(MouseKey key)`
- `IsMouseUp(MouseKey key)`
- `IsMouse(MouseKey key)`
- `IsAnyMouseDown(MouseKey* key)`
- `IsAnyMouseUp(MouseKey* key)`
- `GetMouseState()`
- `LockMouseAtCenter(bool value)`

## **Notes:**

- `IsAnyKeyDown`, `IsAnyKeyUp`, `IsAnyMouseDown` `IsAnyMouseUp` | Check if a key/mouse button is up/down. They return a bool and they accept an optional pointer to a value of type `KeyboardKey` and `MouseKey` which in case of the **true return** that pointer passed as a paramter will be filled with the key/mouse button that triggered the event.

- `LockMouseAtCenter` locks the cursor at the center of the screen and makes it invisible.

- `GetMouseState` return a value of type `MouseState` which contains information about the mouse position and where it is moving to.

## **Examples:**

    if(Input::IsKeyDown(KeyboardKey::KEY_W))
    {
        std::cout << "I am moving forward!" << std::endl;
    }
---
    KeyboardKey key;
    if(Input::IsAnyKeyDown(&key))
    {
        std::cout << "Keycode pressed is: " << static_cast<uint32_t>(key) << std::endl;
    }