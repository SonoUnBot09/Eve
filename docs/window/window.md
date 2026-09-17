# **Window Class:**

The window class in `include/window` allows the user to manage the **Window**.

## **Methods:**

- SetFullscreen(bool) 
- SetBordered(bool)
- SetResizable(bool)
- SetResolution(x, y)
- SetPosition(x, y)
- SetPositionAtCenter()
- SetWindowTitle(title)
- GetWindowSize()
- SetIconPNG(path)

## **Notes:**

- **Do not call this methods every frames as they are exepensive since they request the OS directly.**