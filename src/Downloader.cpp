#include "Downloader.h"
#include <wininet.h>
#include <fstream>

#pragma comment(lib, "wininet.lib")

Downloader::Downloader()
    : m_isDownloading(false)
    , m_cancelled(false)
{
}

Downloader::~Downloader() {
    Cancel();
}

void CALLBACK Downloader::InternetStatusCallback(
    HINTERNET hInternet,
    DWORD_PTR dwContext,
    DWORD dwInternetStatus,
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
) {
    // Status callback for monitoring
}

bool Downloader::DownloadFile(
    const std::wstring& url,
    const std::wstring& destPath,
    ProgressCallback callback
) {
    m_isDownloading = true;
    m_cancelled = false;
    
    bool success = false;
    
    HINTERNET hInternet = InternetOpenW(
        L"NexConnect Setup/1.0",
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0
    );
    
    if (!hInternet) {
        m_isDownloading = false;
        return false;
    }
    
    if (callback) {
        callback(0, L"Connecting...");
    }
    
    HINTERNET hUrl = InternetOpenUrlW(
        hInternet,
        url.c_str(),
        NULL,
        0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );
    
    if (hUrl) {
        // Get file size
        DWORD fileSize = 0;
        DWORD bufferSize = sizeof(DWORD);
        HttpQueryInfoW(
            hUrl,
            HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
            &fileSize,
            &bufferSize,
            NULL
        );
        
        if (callback) {
            callback(5, L"Downloading...");
        }
        
        // Open destination file
        std::ofstream outFile(destPath, std::ios::binary);
        
        if (outFile.is_open()) {
            const DWORD BUFFER_SIZE = 8192;
            BYTE buffer[BUFFER_SIZE];
            DWORD bytesRead = 0;
            DWORD totalBytesRead = 0;
            
            success = true;
            
            while (!m_cancelled) {
                if (!InternetReadFile(hUrl, buffer, BUFFER_SIZE, &bytesRead)) {
                    success = false;
                    break;
                }
                
                if (bytesRead == 0) {
                    break; // Done
                }
                
                outFile.write((char*)buffer, bytesRead);
                totalBytesRead += bytesRead;
                
                if (callback && fileSize > 0) {
                    int progress = (int)((totalBytesRead * 90.0) / fileSize) + 5;
                    if (progress > 95) progress = 95;
                    
                    wchar_t status[256];
                    swprintf_s(status, L"Downloaded: %.2f MB / %.2f MB",
                        totalBytesRead / 1048576.0,
                        fileSize / 1048576.0
                    );
                    callback(progress, status);
                }
            }
            
            outFile.close();
            
            if (m_cancelled) {
                DeleteFileW(destPath.c_str());
                success = false;
            }
        }
        
        InternetCloseHandle(hUrl);
    }
    
    InternetCloseHandle(hInternet);
    m_isDownloading = false;
    
    if (success && callback) {
        callback(100, L"Download complete!");
    }
    
    return success;
}

void Downloader::Cancel() {
    m_cancelled = true;
}
