cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Eve LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.10
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)

FetchContent_MakeAvailable(SDL3 glm)

find_package(
    Vulkan REQUIRED
    COMPONENTS
    volk
    shaderc_combined
)


add_executable(Eve
    main.cpp
    source/Application.cpp
    source/EntityCommandInfo.cpp
    source/EntityCommandPool.cpp
    source/EntityManager.cpp
    source/MemoryLayout.cpp
    source/Table.cpp
)

target_compile_features(Eve PRIVATE cxx_std_20)
target_link_libraries(Eve PRIVATE
    Vulkan::Vulkan
    Vulkan::volk
    Vulkan::shaderc_combined
    SDL3::SDL3
    glm::glm
)

target_include_directories(Eve
    PRIVATE third-parties
    PRIVATE source
    PUBLIC include
)