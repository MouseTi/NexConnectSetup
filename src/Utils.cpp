#include "Utils.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wininet.h>
#include <fstream>
#include <filesystem>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wininet.lib")

namespace fs = std::filesystem;

std::wstring Utils::GetAppDataPath() {
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path);
    
    if (SUCCEEDED(hr)) {
        std::wstring result(path);
        CoTaskMemFree(path);
        return result;
    }
    return L"";
}

std::wstring Utils::GetDesktopPath() {
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path);
    
    if (SUCCEEDED(hr)) {
        std::wstring result(path);
        CoTaskMemFree(path);
        return result;
    }
    return L"";
}

std::wstring Utils::GetTempPath() {
    wchar_t tempPath[MAX_PATH];
    DWORD result = ::GetTempPathW(MAX_PATH, tempPath);
    
    if (result > 0 && result < MAX_PATH) {
        return std::wstring(tempPath);
    }
    return L"";
}

bool Utils::CreateDirectoryRecursive(const std::wstring& path) {
    try {
        fs::create_directories(path);
        return true;
    } catch (...) {
        return false;
    }
}

bool Utils::FileExists(const std::wstring& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool Utils::DirectoryExists(const std::wstring& path) {
    return fs::exists(path) && fs::is_directory(path);
}

std::wstring Utils::StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string Utils::WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

bool Utils::CreateShortcut(
    const std::wstring& targetPath,
    const std::wstring& shortcutPath,
    const std::wstring& description,
    const std::wstring& workingDir
) {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return false;

    IShellLinkW* pShellLink = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&pShellLink);
    
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(targetPath.c_str());
        pShellLink->SetDescription(description.c_str());
        pShellLink->SetWorkingDirectory(workingDir.c_str());

        IPersistFile* pPersistFile = nullptr;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
        
        if (SUCCEEDED(hr)) {
            hr = pPersistFile->Save(shortcutPath.c_str(), TRUE);
            pPersistFile->Release();
        }
        
        pShellLink->Release();
    }

    CoUninitialize();
    return SUCCEEDED(hr);
}

bool Utils::LaunchProcess(const std::wstring& exePath) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL success = CreateProcessW(
        exePath.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }

    return false;
}

bool Utils::ExtractZip(const std::wstring& zipPath, const std::wstring& destPath) {
    // This is a simplified version
    // For production, you'd want to use a proper zip library or Windows Shell
    
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) return false;

    IShellDispatch* pISD = nullptr;
    hr = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void**)&pISD);
    
    bool result = false;
    if (SUCCEEDED(hr) && pISD) {
        // Create destination folder if it doesn't exist
        CreateDirectoryRecursive(destPath);

        VARIANT vZipFile, vDestFolder;
        VariantInit(&vZipFile);
        VariantInit(&vDestFolder);

        vZipFile.vt = VT_BSTR;
        vZipFile.bstrVal = SysAllocString(zipPath.c_str());
        
        vDestFolder.vt = VT_BSTR;
        vDestFolder.bstrVal = SysAllocString(destPath.c_str());

        Folder* pZipFolder = nullptr;
        Folder* pDestFolder = nullptr;

        if (SUCCEEDED(pISD->NameSpace(vZipFile, &pZipFolder)) && pZipFolder) {
            if (SUCCEEDED(pISD->NameSpace(vDestFolder, &pDestFolder)) && pDestFolder) {
                FolderItems* pItems = nullptr;
                if (SUCCEEDED(pZipFolder->Items(&pItems)) && pItems) {
                    VARIANT vItem;
                    VariantInit(&vItem);
                    vItem.vt = VT_DISPATCH;
                    vItem.pdispVal = pItems;

                    VARIANT vOptions;
                    VariantInit(&vOptions);
                    vOptions.vt = VT_I4;
                    vOptions.lVal = 0x14; // No progress dialog, Yes to all

                    pDestFolder->CopyHere(vItem, vOptions);
                    result = true;

                    pItems->Release();
                }
                pDestFolder->Release();
            }
            pZipFolder->Release();
        }

        VariantClear(&vZipFile);
        VariantClear(&vDestFolder);
        pISD->Release();
    }

    CoUninitialize();
    return result;
}

bool Utils::IsInternetConnected() {
    DWORD flags;
    return InternetGetConnectedState(&flags, 0) != 0;
}
