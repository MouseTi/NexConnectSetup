// Simple NexConnect Installer - No Qt dependencies
// Pure Win32 API implementation

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

const wchar_t* APP_NAME = L"NexConnect";
const wchar_t* WINDOW_CLASS = L"NexConnectSetupClass";
const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 300;

struct FileToDownload {
    std::wstring url;
    std::wstring filename;
    DWORD size;
};

struct InstallerState {
    HWND hwnd;
    HWND progressBar;
    HWND statusText;
    std::wstring installPath;
    std::vector<FileToDownload> files;
    int currentFile;
    int progress;
    bool isComplete;
};

InstallerState g_state;

// Get AppData\Local\NexConnect path
std::wstring GetInstallPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        std::wstring installPath = path;
        installPath += L"\\NexConnect";
        return installPath;
    }
    return L"";
}

// Create directory recursively
bool CreateDirectoryRecursive(const std::wstring& path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }
    
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        CreateDirectoryRecursive(path.substr(0, pos));
    }
    
    return CreateDirectoryW(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

// Download file from URL
bool DownloadFile(const std::wstring& url, const std::wstring& destPath, HWND progressBar, HWND statusText) {
    HINTERNET hInternet = InternetOpenW(L"NexConnect-Setup/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetOpenUrlW(hInternet, url.c_str(), NULL, 0, 
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD fileSize = 0;
    DWORD sizeLen = sizeof(fileSize);
    HttpQueryInfoW(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
        &fileSize, &sizeLen, NULL);

    std::ofstream outFile(destPath, std::ios::binary);
    if (!outFile.is_open()) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    DWORD totalRead = 0;
    BYTE buffer[8192];
    DWORD bytesRead;

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outFile.write((char*)buffer, bytesRead);
        totalRead += bytesRead;

        if (fileSize > 0) {
            int percent = (int)((totalRead * 100) / fileSize);
            SendMessage(progressBar, PBM_SETPOS, percent, 0);
            
            wchar_t status[256];
            swprintf_s(status, L"Downloading: %.1f MB / %.1f MB", 
                totalRead / 1024.0 / 1024.0, fileSize / 1024.0 / 1024.0);
            SetWindowTextW(statusText, status);
        }
    }

    outFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    
    return true;
}

// Create desktop shortcut
bool CreateDesktopShortcut(const std::wstring& targetPath, const std::wstring& linkName) {
    CoInitialize(NULL);
    
    IShellLinkW* pShellLink = NULL;
    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellLinkW, (LPVOID*)&pShellLink);
    
    if (SUCCEEDED(hres)) {
        pShellLink->SetPath(targetPath.c_str());
        
        std::wstring workDir = targetPath.substr(0, targetPath.find_last_of(L"\\/"));
        pShellLink->SetWorkingDirectory(workDir.c_str());
        pShellLink->SetDescription(L"NexConnect Launcher");

        IPersistFile* pPersistFile = NULL;
        hres = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        
        if (SUCCEEDED(hres)) {
            wchar_t desktopPath[MAX_PATH];
            SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath);
            
            std::wstring shortcutPath = desktopPath;
            shortcutPath += L"\\";
            shortcutPath += linkName;
            shortcutPath += L".lnk";
            
            pPersistFile->Save(shortcutPath.c_str(), TRUE);
            pPersistFile->Release();
        }
        
        pShellLink->Release();
    }
    
    CoUninitialize();
    return SUCCEEDED(hres);
}

// Installation thread
DWORD WINAPI InstallThread(LPVOID param) {
    InstallerState* state = (InstallerState*)param;
    
    // Step 1: Create directory
    SetWindowTextW(state->statusText, L"Creating installation directory...");
    SendMessage(state->progressBar, PBM_SETPOS, 5, 0);
    Sleep(500);
    
    if (!CreateDirectoryRecursive(state->installPath)) {
        MessageBoxW(state->hwnd, L"Failed to create installation directory", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Step 2: Download files
    state->files = {
        {L"https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.exe", 
         L"NexConnect.exe", 5 * 1024 * 1024},
        {L"https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/nexus_runtime.dll", 
         L"nexus_runtime.dll", 2 * 1024 * 1024}
    };

    for (size_t i = 0; i < state->files.size(); i++) {
        std::wstring destPath = state->installPath + L"\\" + state->files[i].filename;
        
        wchar_t status[256];
        swprintf_s(status, L"Downloading %s (%d/%d)...", 
            state->files[i].filename.c_str(), (int)i + 1, (int)state->files.size());
        SetWindowTextW(state->statusText, status);
        
        int baseProgress = 20 + (i * 60 / state->files.size());
        SendMessage(state->progressBar, PBM_SETPOS, baseProgress, 0);
        
        if (!DownloadFile(state->files[i].url, destPath, state->progressBar, state->statusText)) {
            std::wstring errMsg = L"Failed to download: " + state->files[i].filename;
            MessageBoxW(state->hwnd, errMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
            return 1;
        }
        
        Sleep(300);
    }

    // Step 3: Create shortcut
    SetWindowTextW(state->statusText, L"Creating desktop shortcut...");
    SendMessage(state->progressBar, PBM_SETPOS, 85, 0);
    Sleep(500);
    
    std::wstring exePath = state->installPath + L"\\NexConnect.exe";
    CreateDesktopShortcut(exePath, L"NexConnect");

    // Step 4: Launch application
    SetWindowTextW(state->statusText, L"Launching NexConnect...");
    SendMessage(state->progressBar, PBM_SETPOS, 95, 0);
    Sleep(500);
    
    ShellExecuteW(NULL, L"open", exePath.c_str(), NULL, state->installPath.c_str(), SW_SHOWNORMAL);

    // Complete
    SetWindowTextW(state->statusText, L"Installation complete!");
    SendMessage(state->progressBar, PBM_SETPOS, 100, 0);
    state->isComplete = true;
    
    Sleep(2000);
    PostMessage(state->hwnd, WM_CLOSE, 0, 0);
    
    return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Title
            CreateWindowW(L"STATIC", L"NexConnect Setup", 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 30, 440, 30, hwnd, NULL, NULL, NULL);
            
            // Status text
            g_state.statusText = CreateWindowW(L"STATIC", L"Initializing...", 
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                30, 80, 440, 60, hwnd, NULL, NULL, NULL);
            
            // Progress bar
            g_state.progressBar = CreateWindowW(PROGRESS_CLASSW, NULL,
                WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                30, 200, 440, 25, hwnd, NULL, NULL, NULL);
            SendMessage(g_state.progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendMessage(g_state.progressBar, PBM_SETPOS, 0, 0);
            
            // Percentage text
            CreateWindowW(L"STATIC", L"0%", 
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                200, 160, 100, 30, hwnd, (HMENU)1001, NULL, NULL);
            
            // Set font
            HFONT hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            SendMessage(g_state.statusText, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            // Start installation
            g_state.hwnd = hwnd;
            g_state.installPath = GetInstallPath();
            g_state.isComplete = false;
            
            CreateThread(NULL, 0, InstallThread, &g_state, 0, NULL);
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hbrBackground = CreateSolidBrush(RGB(11, 14, 20));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    RegisterClassW(&wc);
    
    // Calculate center position
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - WINDOW_WIDTH) / 2;
    int y = (screenHeight - WINDOW_HEIGHT) / 2;
    
    // Create window
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        WINDOW_CLASS, L"NexConnect Setup",
        WS_POPUP | WS_VISIBLE,
        x, y, WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL);
    
    if (!hwnd) return 1;
    
    // Set window opacity
    SetLayeredWindowAttributes(hwnd, 0, 250, LWA_ALPHA);
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
