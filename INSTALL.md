# NexConnect Setup - Installation Guide

## For Users

### Installation

1. Download `NexConnect-Setup.exe`
2. Run the installer
3. Click "Install"
4. Wait for the download and installation to complete
5. NexConnect will launch automatically

### Installation Location

NexConnect will be installed to:
```
%LOCALAPPDATA%\NexConnect
```

A desktop shortcut will be created automatically.

### System Requirements

- Windows 10 or Windows 11
- 64-bit processor
- Internet connection for initial download
- ~500 MB free disk space

### Uninstallation

To uninstall NexConnect:
1. Delete the desktop shortcut
2. Delete the folder: `%LOCALAPPDATA%\NexConnect`

## For Developers

### Building from Source

See [README.md](README.md) for detailed build instructions.

### Quick Build

```powershell
.\build.ps1
```

This will create `NexConnect-Setup.exe` in the project root.

### Customization

1. **Change Download URL**: Edit `src/main.cpp` line with `DOWNLOAD_URL`
2. **Change Colors**: Edit color definitions in `src/UI.cpp`
3. **Change Window Size**: Edit dimensions in `src/UI.cpp` in `Initialize()` method
4. **Add Icon**: Add `icon.ico` to `src/` and uncomment icon line in `src/resources.rc`

### Project Structure

```
NexConnectSetup/
├── src/
│   ├── main.cpp           # Entry point and application logic
│   ├── UI.cpp/h           # User interface implementation
│   ├── Installer.cpp/h    # Installation logic
│   ├── Downloader.cpp/h   # HTTP download functionality
│   ├── Utils.cpp/h        # Utility functions
│   ├── resources.rc       # Windows resources
│   └── resource.h         # Resource definitions
├── CMakeLists.txt         # CMake build configuration
├── build.ps1              # Build script
├── README.md              # Main documentation
└── INSTALL.md             # This file
```

### Dependencies

All dependencies are Windows system libraries:
- `wininet.lib` - HTTP downloads
- `shell32.lib` - Shell operations
- `ole32.lib` - COM interfaces
- `comctl32.lib` - Common controls
- `shlwapi.lib` - Shell lightweight API
- `dwmapi.lib` - Desktop Window Manager

No external libraries required!

## Troubleshooting

### Installation Fails

- Check internet connection
- Ensure you have write permissions to `%LOCALAPPDATA%`
- Try running as administrator
- Check firewall settings

### Shortcut Not Created

- Check desktop permissions
- Manually create shortcut to: `%LOCALAPPDATA%\NexConnect\NexConnect.exe`

### Application Won't Launch

- Verify `NexConnect.exe` exists in `%LOCALAPPDATA%\NexConnect`
- Check Windows Event Viewer for errors
- Try reinstalling

## Support

For issues and support, please visit:
- GitHub: https://github.com/MouseTi/NexConnectSetup/issues
