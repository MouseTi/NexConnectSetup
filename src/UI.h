#ifndef UI_H
#define UI_H

#include <windows.h>
#include <string>
#include <functional>

class UI {
public:
    using InstallCallback = std::function<void()>;
    using CancelCallback = std::function<void()>;
    
    UI(HINSTANCE hInstance);
    ~UI();
    
    bool Initialize();
    int Run();
    
    void SetProgress(int progress);
    void SetStatus(const std::wstring& status);
    void ShowError(const std::wstring& message);
    void ShowSuccess(const std::wstring& message);
    void EnableControls(bool enable);
    
    void SetInstallCallback(InstallCallback callback) { m_installCallback = callback; }
    void SetCancelCallback(CancelCallback callback) { m_cancelCallback = callback; }
    
    HWND GetWindow() const { return m_hwnd; }
    
private:
    HINSTANCE m_hInstance;
    HWND m_hwnd;
    HWND m_progressBar;
    HWND m_statusLabel;
    HWND m_installButton;
    HWND m_cancelButton;
    HWND m_titleLabel;
    
    HFONT m_fontTitle;
    HFONT m_fontNormal;
    HFONT m_fontButton;
    
    HBRUSH m_bgBrush;
    HBRUSH m_darkBrush;
    
    InstallCallback m_installCallback;
    CancelCallback m_cancelCallback;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    void CreateControls();
    void SetupFonts();
    void SetDarkMode();
    void DrawBackground(HDC hdc, RECT rect);
};

#endif // UI_H
