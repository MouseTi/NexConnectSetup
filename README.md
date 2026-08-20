# NexConnect Setup

Single-file installer for NexConnect Launcher.

## Features

- **Single EXE**: No DLLs required, everything is statically linked
- **Auto-install**: Automatically creates `%LOCALAPPDATA%\NexConnect` directory
- **Auto-download**: Downloads all required files from GitHub releases
- **Desktop shortcut**: Creates a shortcut on the desktop
- **Auto-launch**: Launches NexConnect.exe after installation

## Building

### Requirements

- Visual Studio 2022
- Qt 6.8.0 (Static build)
- CMake 3.16+

### Build Instructions

1. Install Qt with static linking support
2. Update `build_setup.bat` with your Qt static installation path
3. Run `build_setup.bat`
4. The final executable will be in `build\Release\NexConnect-Setup.exe`

### Building Qt Static (if needed)

```bash
# Download Qt source
# Configure with static options
configure -static -release -prefix "C:\Qt\6.8.0\msvc2022_64_static" ^
    -qt-zlib -qt-libpng -qt-libjpeg ^
    -opengl desktop -nomake examples -nomake tests

# Build (this takes hours)
cmake --build . --parallel
cmake --install .
```

## Usage

1. Build the setup executable
2. Upload NexConnect files to GitHub releases
3. Update `manifest.json` with correct URLs and file information
4. Distribute `NexConnect-Setup.exe` to users
5. Users run the single EXE file - that's it!

## How It Works

1. Shows animated splash screen (2 seconds)
2. Creates installation directory in `%LOCALAPPDATA%\NexConnect`
3. Downloads manifest from GitHub
4. Downloads all files listed in manifest
5. Creates desktop shortcut
6. Launches NexConnect.exe
7. Closes setup window

## Installation Path

Files are installed to: `C:\Users\<Username>\AppData\Local\NexConnect\`

## Manifest Format

```json
{
  "version": "1.0.0",
  "files": [
    {
      "url": "https://github.com/user/repo/releases/download/v1.0.0/file.exe",
      "filename": "file.exe",
      "size": 5242880,
      "sha256": "optional-hash"
    }
  ],
  "shortcut": {
    "name": "NexConnect",
    "description": "NexConnect Launcher"
  },
  "autoLaunch": true
}
```

## License

Copyright (C) 2026 NexConnect
