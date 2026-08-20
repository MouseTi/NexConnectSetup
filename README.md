# NexConnect Setup

Ứng dụng cài đặt tự động cho NexConnect - tải xuống và cài đặt phiên bản mới nhất từ GitHub releases.

## Tính năng

- ✅ Tự động tải xuống NexConnect phiên bản mới nhất từ GitHub
- ✅ Tạo thư mục cài đặt tại `%APPDATA%\NexConnect`
- ✅ Xác thực SHA256 cho tất cả file tải về
- ✅ Tạo shortcut trên Desktop tự động
- ✅ Tự động khởi chạy NexConnect sau khi cài đặt
- ✅ Giao diện splash screen hiện đại

## Cách build

### Yêu cầu

- CMake 3.16 trở lên
- Qt 6.x (Core, Widgets, Network)
- Visual Studio 2019/2022 hoặc MinGW trên Windows

### Build steps

```powershell
# Tạo thư mục build
mkdir build
cd build

# Configure với CMake
cmake .. -G "Visual Studio 17 2022" -A x64

# Build Release
cmake --build . --config Release

# File exe sẽ nằm trong build\Release\NexConnectSetup.exe
```

## Cách hoạt động

1. **Kiểm tra phiên bản**: Tải `manifest.json` từ GitHub releases/latest
2. **Tải file**: Tải `NexConnect.exe` và các file hỗ trợ (như `nexus_runtime.dll`)
3. **Xác thực**: Kiểm tra SHA256 checksum của mọi file
4. **Cài đặt**: Copy file vào `%APPDATA%\NexConnect`
5. **Tạo shortcut**: Tạo shortcut trên Desktop
6. **Khởi chạy**: Tự động mở NexConnect.exe

## Thư mục cài đặt

```
%APPDATA%\NexConnect\
├── NexConnect.exe
├── nexus_runtime.dll
└── (các file khác từ manifest)
```

## GitHub Releases

Setup tự động tải từ:
```
https://github.com/MouseTi/NexConnect/releases/latest/download/manifest.json
```

Manifest chứa:
- `version`: Phiên bản (VD: "1.0.5")
- `url`: Link tải NexConnect.exe
- `sha256`: Checksum của exe
- `files[]`: Danh sách file hỗ trợ cần tải

## License

Copyright (C) 2026 NexConnect
