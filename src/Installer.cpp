#include "Installer.h"
#include "Utils.h"
#include "Downloader.h"
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

Installer::Installer()
    : m_isInstalling(false)
    , m_cancelled(false)
{
    std::wstring appData = Utils::GetAppDataPath();
    m_installPath = appData + L"\\NexConnect";
    m_tempZipPath = Utils::GetTempPath() + L"NexConnect.zip";
}

Installer::~Installer() {
    Cancel();
    Cleanup();
}

std::wstring Installer::GetExePath() const {
    return m_installPath + L"\\NexConnect.exe";
}

bool Installer::Install(
    const std::wstring& downloadUrl,
    ProgressCallback callback
) {
    m_isInstalling = true;
    m_cancelled = false;
    
    bool success = true;
    
    // Step 1: Create install directory
    if (callback) callback(0, L"Creating installation directory...");
    if (!CreateInstallDirectory()) {
        if (callback) callback(0, L"Failed to create installation directory!");
        m_isInstalling = false;
        return false;
    }
    
    // Step 2: Download package
    if (callback) callback(10, L"Downloading NexConnect...");
    if (!DownloadPackage(downloadUrl, callback)) {
        if (callback) callback(0, L"Download failed!");
        m_isInstalling = false;
        return false;
    }
    
    if (m_cancelled) {
        m_isInstalling = false;
        return false;
    }
    
    // Step 3: Extract package
    if (callback) callback(60, L"Extracting files...");
    if (!ExtractPackage(callback)) {
        if (callback) callback(0, L"Extraction failed!");
        m_isInstalling = false;
        return false;
    }
    
    if (m_cancelled) {
        m_isInstalling = false;
        return false;
    }
    
    // Step 4: Create desktop shortcut
    if (callback) callback(85, L"Creating desktop shortcut...");
    if (!CreateDesktopShortcut()) {
        if (callback) callback(85, L"Warning: Could not create shortcut");
        // Not critical, continue
    }
    
    // Step 5: Cleanup temp files
    if (callback) callback(95, L"Cleaning up...");
    Cleanup();
    
    if (callback) callback(100, L"Installation complete!");
    
    // Step 6: Launch application
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    LaunchApplication();
    
    m_isInstalling = false;
    return true;
}

void Installer::Cancel() {
    m_cancelled = true;
}

bool Installer::CreateInstallDirectory() {
    if (Utils::DirectoryExists(m_installPath)) {
        // Directory exists, maybe update?
        return true;
    }
    
    return Utils::CreateDirectoryRecursive(m_installPath);
}

bool Installer::DownloadPackage(const std::wstring& url, ProgressCallback callback) {
    Downloader downloader;
    
    auto downloadCallback = [callback](int progress, const std::wstring& status) {
        if (callback) {
            // Map download progress to 10-60% of total installation
            int mappedProgress = 10 + (progress * 50 / 100);
            callback(mappedProgress, status);
        }
    };
    
    return downloader.DownloadFile(url, m_tempZipPath, downloadCallback);
}

bool Installer::ExtractPackage(ProgressCallback callback) {
    if (!Utils::FileExists(m_tempZipPath)) {
        return false;
    }
    
    bool success = Utils::ExtractZip(m_tempZipPath, m_installPath);
    
    if (success && callback) {
        callback(80, L"Extraction complete!");
    }
    
    return success;
}

bool Installer::CreateDesktopShortcut() {
    std::wstring desktopPath = Utils::GetDesktopPath();
    if (desktopPath.empty()) return false;
    
    std::wstring shortcutPath = desktopPath + L"\\NexConnect.lnk";
    std::wstring exePath = GetExePath();
    
    if (!Utils::FileExists(exePath)) {
        return false;
    }
    
    return Utils::CreateShortcut(
        exePath,
        shortcutPath,
        L"NexConnect - Your Connection Hub",
        m_installPath
    );
}

bool Installer::LaunchApplication() {
    std::wstring exePath = GetExePath();
    
    if (!Utils::FileExists(exePath)) {
        return false;
    }
    
    return Utils::LaunchProcess(exePath);
}

void Installer::Cleanup() {
    if (Utils::FileExists(m_tempZipPath)) {
        DeleteFileW(m_tempZipPath.c_str());
    }
}
