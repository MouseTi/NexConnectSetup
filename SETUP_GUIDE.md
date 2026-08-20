# Hướng dẫn Setup & Push lên GitHub

## 📦 Cấu trúc Project

```
NexConnect-Setup/
├── src/
│   ├── main.cpp              # Entry point
│   ├── SplashScreen.cpp/h    # Màn hình loading
│   ├── SetupManager.cpp/h    # Quản lý download & cài đặt
│   └── resources.rc          # Version info
├── CMakeLists.txt            # Build configuration
├── build.bat                 # Script build Windows
├── .gitignore
└── README.md
```

## 🚀 Tính năng

✅ **Tự động tải về NexConnect** từ GitHub releases/latest  
✅ **Cài đặt vào** `%APPDATA%\NexConnect`  
✅ **Xác thực SHA256** cho mọi file tải về  
✅ **Tạo Desktop Shortcut** tự động  
✅ **Tự động chạy NexConnect.exe** sau khi cài xong  
✅ **Splash screen hiện đại** trong lúc cài đặt  

## 🔨 Build Project

### Yêu cầu
- Windows 10/11
- Qt 6.x (Core, Widgets, Network)
- CMake 3.16+
- Visual Studio 2019/2022 hoặc MinGW

### Build bằng Visual Studio

```powershell
# Chạy script build
.\build.bat
```

Hoặc build thủ công:

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

File output: `build\Release\NexConnectSetup.exe`

## 📤 Push lên GitHub

### Cách 1: Sử dụng GitHub CLI (Khuyến nghị)

```powershell
# Cài GitHub CLI
winget install --id GitHub.cli

# Hoặc chạy script tự động
.\push-to-github.ps1
```

### Cách 2: Setup SSH Key

```bash
# 1. Generate SSH key
ssh-keygen -t ed25519 -C "your_email@example.com"

# 2. Copy public key
cat ~/.ssh/id_ed25519.pub

# 3. Add to GitHub: https://github.com/settings/ssh/new

# 4. Test connection
ssh -T git@github.com

# 5. Push code
git push -u origin main
```

### Cách 3: Personal Access Token

```bash
# 1. Create token: https://github.com/settings/tokens/new
#    - Select "repo" scope

# 2. Change remote to HTTPS
git remote set-url origin https://github.com/MouseTi/NexConnectSetup.git

# 3. Push (sẽ hỏi username & token)
git push -u origin main
```

## 📝 Cách hoạt động

1. **Khởi động**: Hiển thị splash screen với loading animation
2. **Kiểm tra**: Tải `manifest.json` từ GitHub releases/latest
3. **Download**: Tải `NexConnect.exe` và `nexus_runtime.dll` theo manifest
4. **Xác thực**: Kiểm tra SHA256 checksum của mỗi file
5. **Cài đặt**: Copy file vào `%APPDATA%\NexConnect`
6. **Shortcut**: Tạo shortcut "NexConnect" trên Desktop
7. **Khởi chạy**: Tự động mở NexConnect.exe

## 🔗 GitHub Manifest

Setup tự động kéo phiên bản mới nhất từ:
```
https://github.com/MouseTi/NexConnect/releases/latest/download/manifest.json
```

Cấu trúc manifest:
```json
{
  "version": "1.0.5",
  "url": "https://github.com/MouseTi/NexConnect/releases/download/v1.0.5/NexConnect.exe",
  "sha256": "56bed3073ca987714cc7fa25bf5d7ae6d2186306558d5b4237c53e008da5f881",
  "files": [
    {
      "path": "nexus_runtime.dll",
      "url": "https://github.com/MouseTi/NexConnect/releases/download/v1.0.5/nexus_runtime.dll",
      "sha256": "37171757aa740fd07f9a32fea4d12d209d89ce4a4723d09e28bbe641e0e424ee"
    }
  ]
}
```

## 🎯 Thư mục cài đặt

```
%APPDATA%\NexConnect\
├── NexConnect.exe
├── nexus_runtime.dll
└── (các file khác từ manifest)
```

## 🛠️ Troubleshooting

### Lỗi "Host key verification failed"
- Chạy: `ssh-keyscan github.com >> ~/.ssh/known_hosts`

### Lỗi "Authentication failed"
- Sử dụng GitHub CLI hoặc SSH key thay vì password

### Build failed
- Kiểm tra Qt đã được cài đúng path
- Đảm bảo CMake có thể tìm thấy Qt6

## 📄 License

Copyright (C) 2026 NexConnect
