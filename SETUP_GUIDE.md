# Hướng Dẫn Setup NexConnect

## Cho người dùng

### Cách cài đặt (CỰC KỲ ĐƠN GIẢN!)

1. Download file `NexConnect-Setup.exe`
2. Chạy file đó
3. Chờ vài phút để nó tự động download và cài đặt
4. Xong! NexConnect sẽ tự động mở

**Lưu ý**: Không cần cài đặt gì thêm, không cần DLL, chỉ cần 1 file EXE duy nhất!

## Cho developer

### Yêu cầu

- Visual Studio 2022
- Qt 6.8.0 Static (cần build static hoặc download)
- CMake 3.16+

### Bước 1: Chuẩn bị Qt Static

Nếu chưa có Qt static build:

```bash
# Download Qt source code từ qt.io
# Extract và mở cmd trong thư mục source

# Configure Qt với static linking
configure.bat -static -static-runtime -release ^
    -prefix "C:\Qt\6.8.0\msvc2022_64_static" ^
    -platform win32-msvc ^
    -qt-zlib -qt-libpng -qt-libjpeg -qt-freetype ^
    -opengl desktop ^
    -nomake examples -nomake tests ^
    -skip qtwebengine

# Build (mất vài giờ, hãy kiên nhẫn!)
cmake --build . --parallel

# Install
cmake --install .
```

### Bước 2: Build Setup

1. Mở `build_setup.bat`
2. Sửa đường dẫn Qt static:
   ```batch
   set QT_STATIC_DIR=C:\Qt\6.8.0\msvc2022_64_static
   ```
3. Chạy `build_setup.bat`
4. File EXE sẽ ở trong `build\Release\NexConnect-Setup.exe`

### Bước 3: Tạo GitHub Release

1. Đi tới GitHub repository: https://github.com/MouseTi/NexConnectSetup
2. Tạo release mới (vd: v1.0.0)
3. Upload các file cần thiết:
   - NexConnect.exe (file chính từ F:\NexConnect)
   - nexus_runtime.dll
   - Các DLL Qt cần thiết (nếu NexConnect.exe không static)
   - Các file khác từ thư mục NexConnect

### Bước 4: Cập nhật Manifest

Mở `manifest.json` và cập nhật URLs:

```json
{
  "version": "1.0.0",
  "files": [
    {
      "url": "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.exe",
      "filename": "NexConnect.exe",
      "size": 5242880
    },
    {
      "url": "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/nexus_runtime.dll",
      "filename": "nexus_runtime.dll",
      "size": 2097152
    }
  ],
  "shortcut": {
    "name": "NexConnect",
    "description": "NexConnect Launcher"
  },
  "autoLaunch": true
}
```

### Bước 5: Upload manifest.json lên GitHub

Commit và push file `manifest.json` lên GitHub repository.

### Bước 6: Test

1. Copy `NexConnect-Setup.exe` ra desktop
2. Chạy thử
3. Kiểm tra xem nó có:
   - Tạo thư mục `C:\Users\<tên>\AppData\Local\NexConnect`
   - Download đúng các file
   - Tạo shortcut trên desktop
   - Tự động chạy NexConnect.exe

## Cấu trúc thư mục

```
NexConnect-Setup/
├── src/
│   ├── main.cpp              # Entry point
│   ├── SplashScreen.h/cpp    # Màn hình splash
│   ├── SetupWindow.h/cpp     # Cửa sổ setup chính
│   ├── SetupManager.h/cpp    # Logic setup
│   ├── DownloadManager.h/cpp # Download files
│   ├── resources.rc          # Icon và version info
│   └── iconapp/
│       └── icon.ico          # Icon của setup
├── manifest.json             # Danh sách file cần download
├── CMakeLists.txt            # CMake config
├── build_setup.bat           # Script build
└── README.md                 # Documentation
```

## Thư mục cài đặt

Setup sẽ tạo thư mục tại:
```
C:\Users\<Username>\AppData\Local\NexConnect\
├── NexConnect.exe
├── nexus_runtime.dll
└── (các file khác)
```

Và tạo shortcut tại:
```
C:\Users\<Username>\Desktop\NexConnect.lnk
```

## Xử lý lỗi thường gặp

### Lỗi: "Cannot find Qt"
- Kiểm tra đường dẫn Qt trong `build_setup.bat`
- Đảm bảo đã cài Qt static build

### Lỗi: "Missing DLL when running"
- Đảm bảo đã build với Qt static
- Kiểm tra CMakeLists.txt có set `BUILD_SHARED_LIBS OFF`

### Lỗi download: "Failed to download"
- Kiểm tra URLs trong `manifest.json`
- Đảm bảo files đã được upload lên GitHub releases
- Kiểm tra kết nối internet

## Tips

- Để giảm kích thước file EXE, có thể:
  - Compile Release mode (không Debug)
  - Strip symbols
  - Dùng UPX để compress (optional)

- Để thêm file mới vào manifest:
  1. Upload file lên GitHub release
  2. Thêm entry mới vào `manifest.json`
  3. Commit và push manifest

- Để update version:
  1. Update version trong `manifest.json`
  2. Update version trong `CMakeLists.txt`
  3. Update version trong `src/resources.rc`
  4. Rebuild

## Support

Nếu có vấn đề, tạo issue tại: https://github.com/MouseTi/NexConnectSetup/issues
