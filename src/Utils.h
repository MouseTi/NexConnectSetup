#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <shlobj.h>
#include <wininet.h>
#include <string>
#include <vector>

class Utils {
public:
    // Path operations
    static std::wstring GetAppDataPath();
    static std::wstring GetDesktopPath();
    static std::wstring GetTempPath();
    static bool CreateDirectoryRecursive(const std::wstring& path);
    static bool FileExists(const std::wstring& path);
    static bool DirectoryExists(const std::wstring& path);
    
    // String operations
    static std::wstring StringToWString(const std::string& str);
    static std::string WStringToString(const std::wstring& wstr);
    
    // Shortcut operations
    static bool CreateShortcut(
        const std::wstring& targetPath,
        const std::wstring& shortcutPath,
        const std::wstring& description,
        const std::wstring& workingDir
    );
    
    // Process operations
    static bool LaunchProcess(const std::wstring& exePath);
    static bool ExtractZip(const std::wstring& zipPath, const std::wstring& destPath);
    
    // Network
    static bool IsInternetConnected();
};

#endif // UTILS_H
