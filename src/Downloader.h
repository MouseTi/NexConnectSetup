#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <windows.h>
#include <string>
#include <functional>

class Downloader {
public:
    using ProgressCallback = std::function<void(int progress, const std::wstring& status)>;
    
    Downloader();
    ~Downloader();
    
    bool DownloadFile(
        const std::wstring& url,
        const std::wstring& destPath,
        ProgressCallback callback = nullptr
    );
    
    void Cancel();
    bool IsDownloading() const { return m_isDownloading; }
    
private:
    bool m_isDownloading;
    bool m_cancelled;
    
    static void CALLBACK InternetStatusCallback(
        HINTERNET hInternet,
        DWORD_PTR dwContext,
        DWORD dwInternetStatus,
        LPVOID lpvStatusInformation,
        DWORD dwStatusInformationLength
    );
};

#endif // DOWNLOADER_H
