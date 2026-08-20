@echo off
echo ================================================
echo NexConnect Setup - NSIS Installer Builder
echo ================================================
echo.

REM Check if NSIS is installed
set NSIS_PATH=C:\Program Files (x86)\NSIS\makensis.exe
if not exist "%NSIS_PATH%" (
    echo ERROR: NSIS not found!
    echo Please install NSIS from: https://nsis.sourceforge.io/Download
    echo.
    pause
    exit /b 1
)

echo NSIS found!
echo.

REM Check if icon exists
if not exist "src\iconapp\nex-logo.ico" (
    echo WARNING: Icon file not found at src\iconapp\nex-logo.ico
    echo The installer will be created without an icon.
    echo.
)

echo Building NexConnect Setup Installer...
echo.

REM Compile the NSIS script
"%NSIS_PATH%" /V4 installer-downloader.nsi

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ================================================
    echo SUCCESS! 
    echo ================================================
    echo.
    echo Setup file created: NexConnect-Setup.exe
    echo.
    echo Next steps:
    echo 1. Upload NexConnect files to GitHub releases
    echo 2. Update URLs in installer-downloader.nsi if needed
    echo 3. Test NexConnect-Setup.exe
    echo 4. Distribute to users!
    echo.
) else (
    echo.
    echo ================================================
    echo BUILD FAILED!
    echo ================================================
    echo.
    echo Please check the errors above.
    echo.
)

pause
