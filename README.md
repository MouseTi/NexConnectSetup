# NexConnect Setup

Modern installer for NexConnect - Your connection hub application.

## Features

- **Single Executable**: No DLL dependencies, just run `NexConnect-Setup.exe`
- **Auto Installation**: Automatically downloads and installs to `%APPDATA%\NexConnect`
- **Desktop Shortcut**: Creates a desktop shortcut automatically
- **Auto Launch**: Launches NexConnect after installation completes
- **Modern UI**: Dark-themed installer with progress tracking
- **Smart Updates**: Detects existing installations

## Building

### Requirements

- Visual Studio 2019 or later (with C++ Desktop Development)
- CMake 3.15+
- Windows 10/11

### Build Instructions

1. Clone the repository:
```bash
git clone git@github.com:MouseTi/NexConnectSetup.git
cd NexConnectSetup
```

2. Create build directory:
```bash
mkdir build
cd build
```

3. Generate Visual Studio project:
```bash
cmake .. -G "Visual Studio 16 2019" -A x64
```

4. Build the project:
```bash
cmake --build . --config Release
```

The output will be `NexConnect-Setup.exe` in the `build/Release` folder.

### Building Single Executable

To ensure all dependencies are statically linked:

1. Make sure to use `/MT` runtime library (already configured in CMakeLists.txt)
2. Build in Release mode
3. The resulting exe will be standalone with no DLL dependencies

## Usage

Simply run `NexConnect-Setup.exe` and click Install. The installer will:

1. Create installation directory in `%APPDATA%\NexConnect`
2. Download the latest NexConnect package from GitHub
3. Extract all files
4. Create a desktop shortcut
5. Launch NexConnect automatically

## Configuration

Edit the download URL in `src/main.cpp`:

```cpp
const wchar_t* DOWNLOAD_URL = L"https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.zip";
```

## License

Copyright (C) 2026 NexConnect

## Architecture

- **UI Layer**: Custom Win32 dark-themed interface
- **Installer Core**: Handles installation logic and file operations
- **Downloader**: HTTP download with progress tracking
- **Utils**: System utilities (paths, shortcuts, zip extraction)

## Technical Details

- **Language**: C++17
- **GUI Framework**: Win32 API (native)
- **HTTP**: WinINet API
- **Compression**: Windows Shell COM API for ZIP extraction
- **Build System**: CMake
