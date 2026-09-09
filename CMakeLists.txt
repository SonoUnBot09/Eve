cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(Eve LANGUAGES C CXX)

# --- Vulkan Dependency ---

find_package(
    Vulkan REQUIRED
)

# -------------------------------------------------------------------------------------------------------------------------------------------------------------------

# --- Fetch Contents ---

include(FetchContent)

set(SLANG_VERSION "2026.17")
if(WIN32)
    set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-windows-x86_64.zip")
else()
    set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-linux-x86_64.tar.gz")
endif()

FetchContent_Declare(slang_binaries URL ${SLANG_URL})
FetchContent_MakeAvailable(slang_binaries)

add_library(Slang::Slang SHARED IMPORTED GLOBAL)
set_target_properties(Slang::Slang PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${slang_binaries_SOURCE_DIR}/include"
)
if(WIN32)
    set_target_properties(Slang::Slang PROPERTIES
        IMPORTED_LOCATION "${slang_binaries_SOURCE_DIR}/bin/slang.dll"
        IMPORTED_IMPLIB "${slang_binaries_SOURCE_DIR}/lib/slang.lib"
    )
else()
    set_target_properties(Slang::Slang PROPERTIES
        IMPORTED_LOCATION "${slang_binaries_SOURCE_DIR}/lib/libslang.so"
    )
endif()

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
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

FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG vulkan-sdk-1.4.350.0
)

FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(SDL3 glm volk vma)

# -------------------------------------------------------------------------------------------------------------------------------------------------------------------

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
)

add_executable(Eve
    main.cpp
    ${SOURCES}
    third-parties/whereami.c
)

target_compile_features(Eve PRIVATE cxx_std_20)

target_link_libraries(Eve 
PUBLIC
    glm::glm
PRIVATE
    GPUOpen::VulkanMemoryAllocator
    volk::volk
    Vulkan::Vulkan
    SDL3::SDL3
    Slang::Slang
)

target_include_directories(Eve
PUBLIC 
    include
PRIVATE
    third-parties
    source
)

target_compile_definitions(Eve PUBLIC GLM_FORCE_LEFT_HANDED GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_QUAT_DATA_XYZW)