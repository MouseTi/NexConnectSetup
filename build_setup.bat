@echo off
echo Building NexConnect Setup (Static Single EXE)...

REM Set paths - adjust these to your Qt static build location
set QT_STATIC_DIR=C:\Qt\6.8.0\msvc2022_64_static
set PATH=%QT_STATIC_DIR%\bin;%PATH%

REM Create build directory
if not exist build mkdir build
cd build

REM Run CMake with static Qt
cmake .. -G "Visual Studio 17 2022" -A x64 ^
    -DCMAKE_PREFIX_PATH=%QT_STATIC_DIR% ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DBUILD_SHARED_LIBS=OFF

REM Build
cmake --build . --config Release

echo.
echo Build complete! Executable is in: build\Release\NexConnect-Setup.exe
echo.
pause
