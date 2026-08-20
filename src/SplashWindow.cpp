#include "SplashWindow.h"
#include <windows.h>
#include <objbase.h>
#include <windowsx.h>
#include <cmath>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WM_TIMER_ANIM (WM_USER + 100)

// Global GDI+ token
static ULONG_PTR g_gdiplusToken = 0;

SplashWindow::SplashWindow(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_hwnd(nullptr)
    , m_font(nullptr)
    , m_bgBrush(nullptr)
    , m_progress(0)
    , m_spinPhase(0.0)
    , m_timerId(0)
{
    m_bgBrush = CreateSolidBrush(RGB(13, 13, 13)); // #0D0D0D
    
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
}

SplashWindow::~SplashWindow() {
    StopAnimation();
    if (m_font) DeleteObject(m_font);
    if (m_bgBrush) DeleteObject(m_bgBrush);
    
    // Shutdown GDI+
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

bool SplashWindow::Initialize() {
    const wchar_t CLASS_NAME[] = L"NexSplashWindow";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = m_bgBrush;
    
    RegisterClassW(&wc);
    
    int width = 370;
    int height = 250;
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;
    
    m_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"NexConnect Setup",
        WS_POPUP,
        x, y, width, height,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );
    
    if (!m_hwnd) {
        return false;
    }
    
    // Set window transparency
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    
    // Create font
    m_font = CreateFontW(
        13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
    
    return true;
}

void SplashWindow::Show() {
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    StartAnimation();
}

void SplashWindow::Hide() {
    StopAnimation();
    ShowWindow(m_hwnd, SW_HIDE);
}

void SplashWindow::Close() {
    StopAnimation();
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

void SplashWindow::SetProgress(int progress) {
    m_progress = progress;
    InvalidateRect(m_hwnd, nullptr, FALSE);
}

void SplashWindow::SetStatus(const std::wstring& status) {
    m_status = status;
    InvalidateRect(m_hwnd, nullptr, FALSE);
}

void SplashWindow::StartAnimation() {
    if (m_timerId == 0) {
        m_timerId = SetTimer(m_hwnd, WM_TIMER_ANIM, 12, nullptr); // 12ms = ~83fps
    }
}

void SplashWindow::StopAnimation() {
    if (m_timerId != 0) {
        KillTimer(m_hwnd, m_timerId);
        m_timerId = 0;
    }
}

LRESULT CALLBACK SplashWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SplashWindow* pThis = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<SplashWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = reinterpret_cast<SplashWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT SplashWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            StopAnimation();
            return 0;
            
        case WM_PAINT:
            OnPaint();
            return 0;
            
        case WM_TIMER:
            if (wParam == WM_TIMER_ANIM) {
                m_spinPhase += 0.012;
                if (m_spinPhase >= 1.0) m_spinPhase = 0.0;
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
            return 0;
            
        case WM_ERASEBKGND:
            return 1; // We handle all painting
    }
    
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void SplashWindow::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);
    
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    
    // Create double buffer
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
    
    // Fill background
    FillRect(hdcMem, &clientRect, m_bgBrush);
    
    // Draw loading animation
    DrawLoading(hdcMem, clientRect);
    
    // Draw "Nex" text in top-left
    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, RGB(255, 255, 255));
    HFONT oldFont = (HFONT)SelectObject(hdcMem, m_font);
    RECT textRect = {16, 12, clientRect.right, 40};
    DrawTextW(hdcMem, L"Nex", -1, &textRect, DT_LEFT | DT_TOP | DT_SINGLELINE);
    
    // Draw status text at bottom
    if (!m_status.empty()) {
        HFONT statusFont = CreateFontW(
            11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );
        SelectObject(hdcMem, statusFont);
        SetTextColor(hdcMem, RGB(180, 180, 180));
        
        RECT statusRect = {20, clientRect.bottom - 40, clientRect.right - 20, clientRect.bottom - 20};
        DrawTextW(hdcMem, m_status.c_str(), -1, &statusRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        DeleteObject(statusFont);
    }
    
    SelectObject(hdcMem, oldFont);
    
    // Copy to screen
    BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, hdcMem, 0, 0, SRCCOPY);
    
    // Cleanup
    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    
    EndPaint(m_hwnd, &ps);
}

void SplashWindow::DrawLoading(HDC hdc, const RECT& clientRect) {
    int cx = (clientRect.right - clientRect.left) / 2;
    int cy = (clientRect.bottom - clientRect.top) / 2;
    int R = 48;
    
    // Use GDI+ for smooth rendering with RoundCap
    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    
    // Define ring rectangle
    Gdiplus::RectF ring((Gdiplus::REAL)(cx - R), (Gdiplus::REAL)(cy - R), 
                        (Gdiplus::REAL)(R * 2), (Gdiplus::REAL)(R * 2));
    
    // Draw background ring (subtle white with alpha 60)
    Gdiplus::Pen bgPen(Gdiplus::Color(60, 255, 255, 255), 7.0f);
    bgPen.SetStartCap(Gdiplus::LineCapRound);
    bgPen.SetEndCap(Gdiplus::LineCapRound);
    graphics.DrawArc(&bgPen, ring, 0.0f, 360.0f);
    
    // Draw spinning arc (bright white, 90 degrees)
    Gdiplus::Pen spinPen(Gdiplus::Color(255, 255, 255, 255), 7.0f);
    spinPen.SetStartCap(Gdiplus::LineCapRound);
    spinPen.SetEndCap(Gdiplus::LineCapRound);
    
    float startAngle = (float)(m_spinPhase * 360.0);
    float sweepAngle = -90.0f; // Negative = counter-clockwise like Qt
    
    graphics.DrawArc(&spinPen, ring, startAngle, sweepAngle);
    
    // Draw progress percentage in center
    wchar_t progressText[32];
    swprintf_s(progressText, L"%d%%", m_progress);
    
    HFONT progressFont = CreateFontW(
        24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
    
    HFONT oldFont = (HFONT)SelectObject(hdc, progressFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    RECT progressRect = {cx - 50, cy - 15, cx + 50, cy + 15};
    DrawTextW(hdc, progressText, -1, &progressRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(progressFont);
}
