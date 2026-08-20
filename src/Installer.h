#ifndef INSTALLER_H
#define INSTALLER_H

#include <windows.h>
#include <objbase.h>
#include <string>
#include <functional>

class Installer {
public:
    using ProgressCallback = std::function<void(int progress, const std::wstring& status)>;
    
    Installer();
    ~Installer();
    
    bool Install(
        const std::wstring& downloadUrl,
        ProgressCallback callback = nullptr
    );
    
    void Cancel();
    bool IsInstalling() const { return m_isInstalling; }
    
    std::wstring GetInstallPath() const { return m_installPath; }
    std::wstring GetExePath() const;
    
private:
    bool m_isInstalling;
    bool m_cancelled;
    std::wstring m_installPath;
    std::wstring m_tempZipPath;
    
    bool CreateInstallDirectory();
    bool DownloadPackage(const std::wstring& url, ProgressCallback callback);
    bool ExtractPackage(ProgressCallback callback);
    bool CreateDesktopShortcut();
    bool LaunchApplication();
    void Cleanup();
};

#endif // INSTALLER_H
