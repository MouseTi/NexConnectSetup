# NexConnect Setup - Build Instructions

## Hiện trạng

Để build được file setup đơn lẻ (single EXE), bạn cần:

1. **Qt với Static Linking** - Hiện tại chưa có Qt static
2. **Visual Studio 2022** - Cần kiểm tra
3. **CMake** - Cần cài đặt

## Giải pháp thay thế (KHUYẾN NGHỊ)

Vì việc build Qt static rất tốn thời gian (vài giờ đồng hồ), tôi đề xuất 2 phương án:

### Phương án 1: Dùng Qt Dynamic + Installer Framework

Thay vì 1 file EXE duy nhất, chúng ta có thể:
- Build với Qt dynamic (nhanh hơn nhiều)
- Đóng gói tất cả DLLs vào 1 file installer duy nhất bằng Qt Installer Framework hoặc Inno Setup
- Người dùng vẫn chỉ chạy 1 file, nhưng nó sẽ extract và cài đặt

### Phương án 2: Build Qt Shared + Deploy với windeployqt

- Build NexConnect-Setup.exe với Qt shared
- Dùng `windeployqt` để tự động copy tất cả DLLs cần thiết
- Dùng tool như Enigma Virtual Box để đóng gói EXE + DLLs thành 1 file duy nhất

### Phương án 3: Build Qt Static (lâu nhất nhưng tốt nhất)

Nếu bạn muốn đi theo phương án này:

1. Download Qt source: https://download.qt.io/archive/qt/6.8/6.8.0/single/
2. Extract và build static (mất 3-6 giờ)
3. Build setup với Qt static

## Hướng dẫn nhanh với Qt Shared (KHUYẾN NGHỊ)

Tôi sẽ tạo version sử dụng Qt shared, sau đó dùng Enigma Virtual Box để pack thành 1 file.

Bạn muốn đi theo phương án nào?

1. Qt Shared + Enigma Virtual Box (nhanh, 30 phút)
2. Qt Shared + Inno Setup installer (nhanh, 1 giờ)
3. Qt Static build (chất lượng cao nhất, 4-6 giờ)

## Yêu cầu hiện tại

Trước khi build, cần cài:
- [ ] CMake (https://cmake.org/download/)
- [ ] Visual Studio 2022 Community (https://visualstudio.microsoft.com/)
- [ ] Qt 6.8.0 hoặc 6.5+ (https://www.qt.io/download)
- [ ] (Optional) Enigma Virtual Box cho phương án 1

Hãy cho tôi biết bạn muốn đi theo hướng nào?
