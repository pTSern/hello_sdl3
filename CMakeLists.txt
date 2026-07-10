cmake_minimum_required(VERSION 3.22)
project(SDL3_Game_Project LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configure SDL3 Build Options before fetching
set(SDL_SHARED ON CACHE BOOL "Build SDL shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Build SDL static library" FORCE)
set(SDL_TESTS OFF CACHE BOOL "Build SDL tests" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "Build SDL examples" FORCE)
set(SDL_COCOA OFF CACHE BOOL "Disable Cocoa (macOS only)" FORCE)
set(SDL_RENDER_D3D ON CACHE BOOL "Enable D3D renderer" FORCE)
set(SDL_RENDER_METAL OFF CACHE BOOL "Disable Metal" FORCE)

# Using FetchContent to get SDL3
include(FetchContent)
message(STATUS "Fetching SDL3...")
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

# Project executable
add_executable(sdl_game
    src/main.cpp
    src/Game.cpp
    src/Game.hpp
    src/Particle.cpp
    src/Particle.hpp
    src/Vector2.hpp
    src/Font.hpp
    src/Font.cpp
)

# Link SDL3
target_link_libraries(sdl_game PRIVATE SDL3::SDL3)

# Enable compiler warnings for our game target only
if(MSVC)
    target_compile_options(sdl_game PRIVATE /W4 /WX)
else()
    target_compile_options(sdl_game PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
target_include_directories(sdl_game PRIVATE src)

# Copy SDL3.dll to the build output directory for Windows executables
if(WIN32)
    add_custom_command(TARGET sdl_game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:sdl_game>
        COMMENT "Copying SDL3 shared library to output directory..."
    )
endif()

# Suppress console on Windows for Release builds, show it for Debug builds
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set_target_properties(sdl_game PROPERTIES WIN32_EXECUTABLE FALSE)
    else()
        set_target_properties(sdl_game PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

# Set DEBUG and RELEASE preprocessor macros
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(sdl_game PRIVATE DEBUG=1 RELEASE=0 THAU=1)
else()
    target_compile_definitions(sdl_game PRIVATE DEBUG=0 RELEASE=1 THAU=0)
endif()
