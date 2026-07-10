@echo off
echo =========================================
echo    Generating compile_commands.json
echo =========================================

REM Configure project with CMake to generate compile_commands.json
echo Configuring CMake...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF

if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %errorlevel%
)

REM Copy the compilation database to the root directory for LSP detection
if exist "build\compile_commands.json" (
    echo Copying compile_commands.json to project root...
    copy /y "build\compile_commands.json" "compile_commands.json" >nul
    echo Success! Neovim LSP will now recognize include files.
) else (
    echo [ERROR] compile_commands.json was not found in the build directory.
)
