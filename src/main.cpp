#include "UI.h"
#include "Installer.h"
#include "Utils.h"
#include <thread>
#include <windows.h>

#pragma comment(lib, "dwmapi.lib")

// GitHub release URL - bạn cần thay đổi link này
const wchar_t* DOWNLOAD_URL = L"https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.zip";

class Application {
public:
    Application(HINSTANCE hInstance)
        : m_ui(hInstance)
        , m_installer()
    {
    }
    
    bool Initialize() {
        if (!m_ui.Initialize()) {
            return false;
        }
        
        m_ui.SetInstallCallback([this]() { OnInstall(); });
        m_ui.SetCancelCallback([this]() { OnCancel(); });
        
        return true;
    }
    
    int Run() {
        return m_ui.Run();
    }
    
private:
    UI m_ui;
    Installer m_installer;
    
    void OnInstall() {
        if (!Utils::IsInternetConnected()) {
            m_ui.ShowError(L"No internet connection detected!\nPlease check your connection and try again.");
            return;
        }
        
        m_ui.EnableControls(false);
        
        std::thread installThread([this]() {
            auto progressCallback = [this](int progress, const std::wstring& status) {
                PostMessage(m_ui.GetWindow(), WM_USER + 1, progress, (LPARAM)new std::wstring(status));
            };
            
            bool success = m_installer.Install(DOWNLOAD_URL, progressCallback);
            
            PostMessage(m_ui.GetWindow(), WM_USER + 2, success ? 1 : 0, 0);
        });
        
        installThread.detach();
    }
    
    void OnCancel() {
        m_installer.Cancel();
        m_ui.EnableControls(true);
        m_ui.SetStatus(L"Installation cancelled");
        m_ui.SetProgress(0);
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
