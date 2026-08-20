@echo off
setlocal

echo ======================================
echo Building NexConnect Setup
echo ======================================
echo.

if not exist build mkdir build
cd build

echo [1/3] Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

echo.
echo [2/3] Building Release...
cmake --build . --config Release
if errorlevel 1 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo [3/3] Build complete!
echo.
echo Output: build\Release\NexConnectSetup.exe
echo.

pause
