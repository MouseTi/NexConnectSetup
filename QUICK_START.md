# NexConnect Setup - Simplified Build Approach

## Vấn đề hiện tại

Để build được 1 file EXE duy nhất (không cần DLL), cần:
- Qt Static build (chưa có, mất 4-6 giờ để build)
- Hoặc dùng công cụ đóng gói như Enigma Virtual Box

## Giải pháp đơn giản nhất - Sử dụng Qt Shared

Tôi đã tạo sẵn code setup, nhưng để build cần:

### Bước 1: Cài đặt Qt (chọn 1 trong 2)

**Option A: Qt Online Installer (Khuyến nghị)**
1. Download: https://www.qt.io/download-qt-installer
2. Cài Qt 6.8.0 MSVC 2022 64-bit
3. Chọn components: Qt WebEngine, Qt Network

**Option B: Build Qt Static (nếu muốn 1 file EXE duy nhất)**
- Mất 4-6 giờ
- Cần 50GB dung lượng
- Cần Visual Studio 2022

### Bước 2: Cài Visual Studio 2022

1. Download: https://visualstudio.microsoft.com/downloads/
2. Chọn "Desktop development with C++"
3. Install

### Bước 3: Cài CMake

1. Download: https://cmake.org/download/
2. Install và thêm vào PATH

### Bước 4: Build

Sau khi cài đặt xong Qt:

```batch
# Sửa đường dẫn Qt trong build_setup.bat
# Từ: set QT_STATIC_DIR=C:\Qt\6.8.0\msvc2022_64_static
# Thành: set QT_STATIC_DIR=C:\Qt\6.8.0\msvc2022_64

# Chạy build
build_setup.bat
```

### Bước 5: Deploy DLLs (nếu dùng Qt Shared)

```batch
cd build\Release
windeployqt NexConnect-Setup.exe
```

### Bước 6: Đóng gói thành 1 file (Optional)

**Option A: Enigma Virtual Box (Free)**
1. Download: https://enigmaprotector.com/en/downloads.html
2. Add NexConnect-Setup.exe và tất cả DLLs
3. Build thành 1 file

**Option B: Inno Setup**
1. Download: https://jrsoftware.org/isdl.php
2. Tạo installer script
3. Compile thành setup.exe

## Hoặc đơn giản hơn - Dùng NSIS để tạo installer

Tôi có thể tạo 1 script NSIS để:
- Pack tất cả files
- User chạy 1 file setup.exe
- Tự động extract và chạy

Bạn muốn tôi:
1. Tạo script NSIS installer (đơn giản nhất)?
2. Tiếp tục với Qt build (cần cài Qt trước)?
3. Hướng dẫn cài đặt môi trường build?
