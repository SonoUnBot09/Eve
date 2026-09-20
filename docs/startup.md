# **How To Start Eve Engine?**

In the `include` folder there is a file called **Eve.hpp**. Use that file to handle the engine startup, run and shutdown.

## **You have three main methods:**

- ### `Initialize(...)`

- ### `Run()`

- ### `Shutdown()`

## **Notes:**

- `Initialize` takes a parameter called **EveEngineCreateInfo** which is used to specify some settings to the engine, where to find shaders, windows dimensions and other settings.

- `Run` executes the main loop and call the `Awake`, `Start` and `Update` methods statically recorded.

- `Shutdown` must be called when the application needs to be closed, it will clear and release all the engine's resources and memory. When `Shutdown` is called it executes all the `Shutdown` stages statically recorded. Any interaction with the engine after `Shutdown` has been called is **UB**.

If you want know more about `Awake`, `Start`, `Update`, and `Shutdown` stages and what they are go to the **Entities** section.

## **WARNING:** 
**Call this methods in order, `Initialize`, `Run` and `Shutdown`.**
If you do not respect the order the program will crash. Any interaction with the engine before calling the method `Initialize` is **UB**. Here is an example on how to properly use this class:

    Eve::EveEngineCreateInfo info
    {
        .ShaderSearchPaths{"shaders"},
        .WindowWidth = 740,
        .WindowHeight = 512,
        .WindowTitle = "Eve"
    };
    
    if(Eve::EveEngine::Initialize(info))
    {
        Eve::EveEngine::Run();
    }

    Eve::EveEngine::Shutdown();

## **Important:**

- Once you compile an executable with Eve is very important you provide Slang's dll to the executable. It needs `slang.dll`, `slang-compiler.dll` and `slang-glslang.dll`. Usually you can find the in the inside `_deps\slang_binaries-src\bin`.

- Every path you give to Eve is based on the executable directory.