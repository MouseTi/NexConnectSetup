#ifndef SPLASHWINDOW_H
#define SPLASHWINDOW_H

#include <windows.h>
#include <string>
#include <functional>

class SplashWindow {
public:
    using ProgressCallback = std::function<void(int progress, const std::wstring& status)>;
    
    SplashWindow(HINSTANCE hInstance);
    ~SplashWindow();
    
    bool Initialize();
    void Show();
    void Hide();
    void SetProgress(int progress);
    void SetStatus(const std::wstring& status);
    void Close();
    
    HWND GetHwnd() const { return m_hwnd; }
    
private:
    HINSTANCE m_hInstance;
    HWND m_hwnd;
    HFONT m_font;
    HBRUSH m_bgBrush;
    
    int m_progress;
    std::wstring m_status;
    double m_spinPhase;
    UINT_PTR m_timerId;
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    void OnPaint();
    void DrawLoading(HDC hdc, const RECT& clientRect);
    void StartAnimation();
    void StopAnimation();
};

#endif // SPLASHWINDOW_H
