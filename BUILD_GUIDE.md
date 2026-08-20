# NexConnect Setup - Build Instructions

## Tóm tắt

Tôi đã tạo **3 phương án** để bạn có thể build setup installer:

## Phương án 1: NSIS Installer (ĐƠN GIẢN NHẤT - KHUYẾN NGHỊ)

### Đây là cách NHANH NHẤT và KHÔNG CẦN Qt!

**Bước 1: Cài NSIS**
1. Download NSIS: https://nsis.sourceforge.io/Download
2. Cài đặt NSIS

**Bước 2: Chuẩn bị files NexConnect**
- Copy tất cả files từ `F:\NexConnect-Setup\NexConnect-Setup\` (NexConnect.exe và các DLLs)
- Hoặc từ thư mục build của NexConnect chính

**Bước 3: Build installer**
```batch
# Right-click vào file installer-downloader.nsi
# Chọn "Compile NSIS Script"

# Hoặc dùng command line:
"C:\Program Files (x86)\NSIS\makensis.exe" installer-downloader.nsi
```

**Kết quả**: File `NexConnect-Setup.exe` sẽ được tạo - đây là file duy nhất người dùng cần!

### Installer này sẽ:
- ✅ Tải xuống NexConnect.exe từ GitHub releases
- ✅ Tải xuống nexus_runtime.dll và các file cần thiết
- ✅ Tạo thư mục %LOCALAPPDATA%\NexConnect
- ✅ Tạo desktop shortcut
- ✅ Tự động chạy NexConnect sau khi cài
- ✅ Có uninstaller trong Control Panel

---

## Phương án 2: Qt Application (PHỨC TẠP HƠN)

Cần cài đặt:
- Qt 6.8.0 MSVC 2022 64-bit
- Visual Studio 2022
- CMake

**Bước build:**
```batch
# Sửa đường dẫn Qt trong build_setup.bat
build_setup.bat
```

---

## Phương án 3: Qt Static Build (CHẤT LƯỢNG CAO NHẤT)

Giống phương án 2 nhưng dùng Qt static (mất 4-6 giờ để build Qt)

---

## So sánh

| Phương án | Thời gian | File size | Độ phức tạp |
|-----------|-----------|-----------|-------------|
| NSIS Downloader | 5 phút | ~500KB | ⭐ Rất dễ |
| Qt Shared | 1-2 giờ | 10-20MB | ⭐⭐⭐ Khó |
| Qt Static | 5-7 giờ | 5-10MB | ⭐⭐⭐⭐⭐ Rất khó |

## KHUYẾN NGHỊ

**Dùng NSIS!** Vì:
1. Nhanh nhất (5 phút)
2. Không cần Qt, Visual Studio
3. File nhỏ gọn (~500KB)
4. Tải files từ GitHub khi cài (giảm bandwidth)
5. Dễ update (chỉ cần up files mới lên GitHub)

## Next Steps với NSIS

1. **Cài NSIS** (link ở trên)
2. **Upload files lên GitHub releases:**
   - Tạo release v1.0.0
   - Upload `NexConnect.exe`
   - Upload `nexus_runtime.dll`
   - Upload các DLLs Qt cần thiết

3. **Compile installer:**
   ```batch
   "C:\Program Files (x86)\NSIS\makensis.exe" installer-downloader.nsi
   ```

4. **Test:** Chạy `NexConnect-Setup.exe` để test

5. **Distribute:** Share file setup cho người dùng!

## Cần giúp gì thêm?

Bạn muốn:
1. Tôi tạo thêm script để package files NexConnect hiện có?
2. Hướng dẫn chi tiết upload lên GitHub releases?
3. Tiếp tục với Qt build?
