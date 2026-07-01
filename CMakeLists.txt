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

if(TARGET Vulkan::shaderc_combined)
    set_target_properties(Vulkan::shaderc_combined PROPERTIES MAP_IMPORTED_CONFIG_DEBUG Release)
endif()

if(TARGET Vulkan::volk)
    set_target_properties(Vulkan::volk PROPERTIES MAP_IMPORTED_CONFIG_DEBUG Release)
endif()

add_executable(Eve
    main.cpp
    source/Application.cpp
    source/EntityCommandInfo.cpp
    source/EntityCommandPool.cpp
    source/EntityManager.cpp
    source/MemoryLayout.cpp
    source/Table.cpp
    source/SystemDispatcher.cpp
    source/S_Test.cpp
)

target_compile_features(Eve PRIVATE cxx_std_20)

set_property(TARGET Eve PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Flag per GCC e Clang
    target_compile_options(Eve PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(Eve PRIVATE -fsanitize=address)

    if(WIN32)
        target_compile_definitions(Eve PRIVATE 
            _DISABLE_STRING_ANNOTATION=1 
            _DISABLE_VECTOR_ANNOTATION=1
        )
    endif()
elseif(MSVC)
    # Flag per MSVC (Visual Studio)
    target_compile_options(Eve PRIVATE /fsanitize=address)
    target_link_options(Eve PRIVATE /fsanitize=address)
endif()

target_link_libraries(Eve PRIVATE
    SDL3::SDL3
    glm::glm
    Vulkan::Vulkan
    Vulkan::volk
    Vulkan::shaderc_combined
)

target_include_directories(Eve
    PRIVATE third-parties
    PRIVATE source
    PUBLIC include
)