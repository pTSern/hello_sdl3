@echo off
setlocal enabledelayedexpansion

set BUILD_TYPE=Release
set DO_CLEAN=false

REM Parse arguments
for %%a in (%*) do (
    if /I "%%a"=="true" (
        set BUILD_TYPE=Debug
    )
    if /I "%%a"=="debug" (
        set BUILD_TYPE=Debug
    )
    if /I "%%a"=="false" (
        set BUILD_TYPE=Release
    )
    if /I "%%a"=="release" (
        set BUILD_TYPE=Release
    )
    if /I "%%a"=="clean" (
        set DO_CLEAN=true
    )
)

echo =========================================
echo       SDL3 Game Project Builder
echo       Mode: !BUILD_TYPE!
echo =========================================

if "!DO_CLEAN!"=="true" (
    echo [CLEAN] Wiping build directory...
    if exist "build" rmdir /s /q build
)

REM We check if build directory exists.
REM If we change configuration, we want to re-run cmake config anyway to update CMAKE_BUILD_TYPE.
REM CMake is smart; running configure on an existing build folder just updates the CMAKE_BUILD_TYPE cache variable.
echo [1/3] Configuring project with CMake (!BUILD_TYPE!)...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=!BUILD_TYPE! -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if !errorlevel! neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b !errorlevel!
)

REM Copy compile_commands.json to root for LSP autocompletion
if exist "build\compile_commands.json" (
    copy /y "build\compile_commands.json" "compile_commands.json" >nul
)

echo [2/3] Compiling and building binaries...
cmake --build build -j 8
if !errorlevel! neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b !errorlevel!
)

echo [3/3] Launching game executable...
echo -----------------------------------------
.\build\sdl_game.exe
echo -----------------------------------------
echo Game exited.
