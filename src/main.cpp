#include <windows.h>
#include <objbase.h>
#include "SplashWindow.h"
#include "Installer.h"
#include "ManifestParser.h"
#include "Utils.h"
#include <thread>

#pragma comment(lib, "ole32.lib")

// Manifest URL - always points to latest version
const wchar_t* MANIFEST_URL = L"https://github.com/MouseTi/NexConnect/releases/latest/download/manifest.json";

class Application {
public:
    Application(HINSTANCE hInstance)
        : m_splash(hInstance)
        , m_installer()
        , m_isInstalling(false)
    {
    }
    
    bool Initialize() {
        if (!m_splash.Initialize()) {
            return false;
        }
        
        return true;
    }
    
    int Run() {
        m_splash.Show();
        
        // Start installation automatically
        StartInstallation();
        
        // Message loop
        MSG msg = {};
        while (GetMessage(&msg, nullptr, 0, 0)) {
            // Handle custom messages for progress updates
            if (msg.message == WM_USER + 1) {
                // Progress update
                int progress = (int)msg.wParam;
                std::wstring* status = (std::wstring*)msg.lParam;
                if (status) {
                    m_splash.SetProgress(progress);
                    m_splash.SetStatus(*status);
                    delete status;
                }
            } else if (msg.message == WM_USER + 2) {
                // Installation complete
                bool success = msg.wParam != 0;
                
                if (success) {
                    m_splash.SetProgress(100);
                    m_splash.SetStatus(L"Complete! Launching...");
                    
                    // Close installer after a short delay
                    std::thread([this]() {
                        Sleep(1500);
                        m_splash.Close();
                        PostQuitMessage(0);
                    }).detach();
                } else {
                    m_splash.SetProgress(0);
                    m_splash.SetStatus(L"Installation failed");
                    MessageBoxW(nullptr, 
                        L"Installation failed!\nPlease check your internet connection and try again.", 
                        L"Error", 
                        MB_OK | MB_ICONERROR);
                    
                    Sleep(1000);
                    PostQuitMessage(1);
                }
            }
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        return (int)msg.wParam;
    }
    
private:
    SplashWindow m_splash;
    Installer m_installer;
    bool m_isInstalling;
    
    void StartInstallation() {
        if (m_isInstalling) return;
        
        if (!Utils::IsInternetConnected()) {
            MessageBoxW(nullptr, 
                L"No internet connection detected!\nPlease check your connection and try again.", 
                L"Error", 
                MB_OK | MB_ICONERROR);
            PostQuitMessage(1);
            return;
        }
        
        // Check if already installed
        std::wstring installPath = m_installer.GetInstallPath();
        std::wstring exePath = installPath + L"\\NexConnect.exe";
        
        if (Utils::FileExists(exePath)) {
            // Already installed, just launch
            m_splash.SetStatus(L"Launching NexConnect...");
            Sleep(500);
            
            if (Utils::LaunchProcess(exePath)) {
                PostQuitMessage(0);
            } else {
                MessageBoxW(nullptr, 
                    L"Failed to launch NexConnect!\nPlease try reinstalling.", 
                    L"Error", 
                    MB_OK | MB_ICONERROR);
                PostQuitMessage(1);
            }
            return;
        }
        
        m_isInstalling = true;
        m_splash.SetStatus(L"Fetching latest version...");
        
        std::thread installThread([this]() {
            auto progressCallback = [this](int progress, const std::wstring& status) {
                PostMessage(m_splash.GetHwnd(), WM_USER + 1, progress, (LPARAM)new std::wstring(status));
            };
            
            // Fetch manifest
            progressCallback(5, L"Checking latest version...");
            ManifestInfo manifest = ManifestParser::ParseFromUrl(MANIFEST_URL);
            
            if (!manifest.isValid || manifest.downloadUrl.empty()) {
                progressCallback(0, L"Failed to fetch version info");
                PostMessage(m_splash.GetHwnd(), WM_USER + 2, 0, 0);
                return;
            }
            
            std::wstring statusMsg = L"Installing NexConnect v" + manifest.version;
            progressCallback(10, statusMsg);
            
            // Install with download URL from manifest
            bool success = m_installer.Install(manifest.downloadUrl, progressCallback);
            
            PostMessage(m_splash.GetHwnd(), WM_USER + 2, success ? 1 : 0, 0);
        });
        
        installThread.detach();
    }
};

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    
    Application app(hInstance);
    
    if (!app.Initialize()) {
        MessageBoxW(nullptr, L"Failed to initialize application!", L"Error", MB_OK | MB_ICONERROR);
        CoUninitialize();
        return 1;
    }
    
    int result = app.Run();
    
    CoUninitialize();
    return result;
}
