# NexConnect Setup Installer

Single-file installer for NexConnect Launcher that automatically downloads and installs the application.

## 🚀 Quick Start (For End Users)

1. Download `NexConnect-Setup.exe`
2. Run it
3. Wait for automatic download and installation
4. Done! NexConnect will launch automatically

**No additional software or DLLs required!**

## 📦 Features

- ✅ **Single file installer** - Just one EXE file
- ✅ **Auto-download** - Downloads files from GitHub releases during installation
- ✅ **Auto-install** - Creates `%LOCALAPPDATA%\NexConnect` directory
- ✅ **Desktop shortcut** - Automatically creates shortcut
- ✅ **Auto-launch** - Launches NexConnect.exe after installation
- ✅ **Uninstaller** - Full uninstall support via Control Panel
- ✅ **Modern UI** - Clean, professional installer interface

## 🛠️ For Developers - Build Instructions

### Method 1: NSIS Installer (RECOMMENDED - 5 minutes)

**Easiest and fastest way!**

#### Requirements
- [NSIS](https://nsis.sourceforge.io/Download) (Nullsoft Scriptable Install System)

#### Steps
1. Install NSIS
2. Run `build_nsis.bat`
3. Done! `NexConnect-Setup.exe` will be created

#### What it does
- Downloads NexConnect files from GitHub releases during installation
- Creates installation directory
- Creates shortcuts
- Registers uninstaller

---

### Method 2: Qt Application (1-2 hours)

**Full-featured setup with animated UI**

#### Requirements
- Qt 6.8.0 (MSVC 2022 64-bit)
- Visual Studio 2022
- CMake 3.16+

#### Steps
1. Install requirements
2. Edit `build_setup.bat` with your Qt path
3. Run `build_setup.bat`
4. Deploy DLLs with `windeployqt`

---

### Method 3: Qt Static Build (5-7 hours)

**Single EXE with no DLL dependencies**

Same as Method 2 but requires building Qt with static linking.

---

## 📊 Comparison

| Method | Time | File Size | Complexity | Single File |
|--------|------|-----------|------------|-------------|
| NSIS | 5 min | ~500KB | ⭐ Easy | ✅ |
| Qt Shared | 1-2 hrs | 15-20MB | ⭐⭐⭐ Hard | ❌ |
| Qt Static | 5-7 hrs | 5-10MB | ⭐⭐⭐⭐⭐ Very Hard | ✅ |

**Recommendation: Use NSIS!**

## 📚 Documentation

- **[SUMMARY.md](SUMMARY.md)** - Complete overview
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Detailed build instructions (3 methods)
- **[UPLOAD_GUIDE.md](UPLOAD_GUIDE.md)** - How to upload files to GitHub releases
- **[SETUP_GUIDE.md](SETUP_GUIDE.md)** - Vietnamese detailed guide
- **[QUICK_START.md](QUICK_START.md)** - Quick start guide

## 🎯 Installation Flow

```
User runs NexConnect-Setup.exe
         ↓
Downloads NexConnect.exe from GitHub
         ↓
Downloads nexus_runtime.dll
         ↓
Downloads required Qt DLLs
         ↓
Creates %LOCALAPPDATA%\NexConnect\
         ↓
Installs all files
         ↓
Creates desktop shortcut
         ↓
Launches NexConnect.exe
         ↓
Done!
```

## 🌐 Upload to GitHub Releases

Before building the installer, you need to:

1. Create a release on GitHub: `v1.0.0`
2. Upload these files:
   - `NexConnect.exe`
   - `nexus_runtime.dll`
   - Required Qt DLLs
3. Update URLs in `installer-downloader.nsi` if needed

See [UPLOAD_GUIDE.md](UPLOAD_GUIDE.md) for detailed instructions.

## 📁 Project Structure

```
NexConnect-Setup/
├── installer-downloader.nsi    # Main NSIS installer script
├── build_nsis.bat             # NSIS build script
├── CMakeLists.txt             # Qt CMake configuration
├── build_setup.bat            # Qt build script
├── manifest.json              # File list for Qt downloader
├── src/
│   ├── main.cpp               # Qt entry point
│   ├── SetupWindow.cpp/h      # Main setup window
│   ├── SetupManager.cpp/h     # Setup logic
│   ├── DownloadManager.cpp/h  # Download handler
│   ├── SplashScreen.cpp/h     # Animated splash screen
│   ├── resources.rc           # Windows resources
│   └── iconapp/
│       └── nex-logo.ico       # Application icon
└── docs/
    ├── BUILD_GUIDE.md
    ├── UPLOAD_GUIDE.md
    └── ...
```

## 🔧 Configuration

### NSIS Installer

Edit `installer-downloader.nsi`:

```nsis
!define APP_NAME "NexConnect"
!define APP_VERSION "1.0.0"
!define DOWNLOAD_URL "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0"
```

### Qt Application

Edit `manifest.json`:

```json
{
  "version": "1.0.0",
  "files": [
    {
      "url": "https://github.com/.../NexConnect.exe",
      "filename": "NexConnect.exe",
      "size": 5242880
    }
  ]
}
```

## 🐛 Troubleshooting

### "Cannot find NSIS"
- Install NSIS from https://nsis.sourceforge.io/Download
- Check path in `build_nsis.bat`

### "Download failed"
- Check internet connection
- Verify GitHub release URLs
- Ensure files are uploaded to GitHub releases

### "Qt not found"
- Install Qt 6.8.0
- Update Qt path in `build_setup.bat`

## 📝 License

Copyright (C) 2026 NexConnect

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## 📧 Support

For issues and questions:
- Open an issue: https://github.com/MouseTi/NexConnectSetup/issues
- Contact: [Your contact info]

## ⭐ Quick Commands

```batch
# Build NSIS installer (recommended)
build_nsis.bat

# Build Qt application
build_setup.bat

# Add files and commit
git add .
git commit -m "Your message"

# Push to GitHub
git push origin main
```

## 🎉 Credits

Built with:
- [NSIS](https://nsis.sourceforge.io/) - Installer creator
- [Qt](https://www.qt.io/) - UI framework
- [CMake](https://cmake.org/) - Build system
