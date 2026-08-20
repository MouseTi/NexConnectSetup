@echo off
echo Building NexConnect Simple Installer (Pure Win32, No Dependencies)
echo.

REM Find Visual Studio 2022
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo ERROR: Visual Studio Installer not found!
    echo Please install Visual Studio 2022 with C++ Desktop Development
    pause
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
    set "VSINSTALLDIR=%%i"
)

if not defined VSINSTALLDIR (
    echo ERROR: Visual Studio 2022 with C++ tools not found!
    echo Please install Visual Studio 2022 with "Desktop development with C++"
    pause
    exit /b 1
)

echo Found Visual Studio at: %VSINSTALLDIR%
echo.

REM Setup Visual Studio environment
call "%VSINSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64

echo.
echo Compiling...
cd /d "%~dp0"

REM Compile the installer
cl.exe /nologo /O2 /MT /EHsc /W3 ^
    /D "UNICODE" /D "_UNICODE" /D "WIN32_LEAN_AND_MEAN" ^
    /Fe:NexConnect-Setup.exe ^
    src\simple_installer.cpp ^
    /link /SUBSYSTEM:WINDOWS ^
    user32.lib gdi32.lib shell32.lib ole32.lib wininet.lib comctl32.lib

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo BUILD SUCCESS!
    echo ========================================
    echo.
    echo Executable created: NexConnect-Setup.exe
    echo Size: 
    dir NexConnect-Setup.exe | findstr "NexConnect-Setup.exe"
    echo.
    echo This is a standalone EXE with NO dependencies!
    echo Users can run it directly without installing anything.
    echo.
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo.
)

pause
