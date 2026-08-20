#include "UI.h"
#include "resource.h"
#include <commctrl.h>
#include <windowsx.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_INSTALL_BUTTON 1001
#define ID_CANCEL_BUTTON 1002

// Dark theme colors
#define COLOR_BG RGB(11, 14, 20)           // #0B0E14
#define COLOR_SURFACE RGB(15, 23, 42)      // #0F172A
#define COLOR_BORDER RGB(30, 41, 59)       // #1E293B
#define COLOR_TEXT RGB(248, 250, 252)      // #F8FAFC
#define COLOR_TEXT_DIM RGB(148, 163, 184)  // #94A3B8
#define COLOR_ACCENT RGB(56, 189, 248)     // #38BDF8 - Cyan accent
#define COLOR_BUTTON RGB(241, 115, 22)     // #F17316 - Orange CTA

UI::UI(HINSTANCE hInstance)
    : m_hInstance(hInstance)
    , m_hwnd(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_installButton(nullptr)
    , m_cancelButton(nullptr)
    , m_titleLabel(nullptr)
    , m_fontTitle(nullptr)
    , m_fontNormal(nullptr)
    , m_fontButton(nullptr)
    , m_bgBrush(nullptr)
    , m_darkBrush(nullptr)
{
    m_bgBrush = CreateSolidBrush(COLOR_BG);
    m_darkBrush = CreateSolidBrush(COLOR_SURFACE);
}

UI::~UI() {
    if (m_fontTitle) DeleteObject(m_fontTitle);
    if (m_fontNormal) DeleteObject(m_fontNormal);
    if (m_fontButton) DeleteObject(m_fontButton);
    if (m_bgBrush) DeleteObject(m_bgBrush);
    if (m_darkBrush) DeleteObject(m_darkBrush);
}

bool UI::Initialize() {
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    const wchar_t CLASS_NAME[] = L"NexConnectSetupWindow";
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = m_bgBrush;
    
    RegisterClassW(&wc);
    
    int width = 600;
    int height = 450;
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;
    
    m_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"NexConnect Setup",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, width, height,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );
    
    if (!m_hwnd) {
        return false;
    }
    
    SetDarkMode();
    SetupFonts();
    CreateControls();
    
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    
    return true;
}

int UI::Run() {
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

void UI::SetupFonts() {
    m_fontTitle = CreateFontW(
        32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
    
    m_fontNormal = CreateFontW(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
    
    m_fontButton = CreateFontW(
        15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
}

void UI::CreateControls() {
    // Title
    m_titleLabel = CreateWindowW(
        L"STATIC",
        L"NexConnect",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 50, 600, 50,
        m_hwnd,
        nullptr,
        m_hInstance,
        nullptr
    );
    SendMessage(m_titleLabel, WM_SETFONT, (WPARAM)m_fontTitle, TRUE);
    
    // Subtitle
    HWND subtitle = CreateWindowW(
        L"STATIC",
        L"Ready to install your connection hub",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 105, 600, 30,
        m_hwnd,
        nullptr,
        m_hInstance,
        nullptr
    );
    SendMessage(subtitle, WM_SETFONT, (WPARAM)m_fontNormal, TRUE);
    
    // Status label
    m_statusLabel = CreateWindowW(
        L"STATIC",
        L"Click Install to begin",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 200, 600, 25,
        m_hwnd,
        nullptr,
        m_hInstance,
        nullptr
    );
    SendMessage(m_statusLabel, WM_SETFONT, (WPARAM)m_fontNormal, TRUE);
    
    // Progress bar
    m_progressBar = CreateWindowW(
        PROGRESS_CLASSW,
        nullptr,
        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
        100, 240, 400, 30,
        m_hwnd,
        nullptr,
        m_hInstance,
        nullptr
    );
    SendMessage(m_progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessage(m_progressBar, PBM_SETPOS, 0, 0);
    
    // Install button
    m_installButton = CreateWindowW(
        L"BUTTON",
        L"Install",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
        200, 310, 200, 45,
        m_hwnd,
        (HMENU)ID_INSTALL_BUTTON,
        m_hInstance,
        nullptr
    );
    SendMessage(m_installButton, WM_SETFONT, (WPARAM)m_fontButton, TRUE);
    
    // Cancel button
    m_cancelButton = CreateWindowW(
        L"BUTTON",
        L"Cancel",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT,
        250, 365, 100, 35,
        m_hwnd,
        (HMENU)ID_CANCEL_BUTTON,
        m_hInstance,
        nullptr
    );
    SendMessage(m_cancelButton, WM_SETFONT, (WPARAM)m_fontNormal, TRUE);
    EnableWindow(m_cancelButton, FALSE);
}

void UI::SetDarkMode() {
    BOOL USE_DARK_MODE = TRUE;
    // DWMWA_USE_IMMERSIVE_DARK_MODE = 20
    typedef HRESULT (WINAPI* DwmSetWindowAttributeFunc)(HWND, DWORD, LPCVOID, DWORD);
    HMODULE hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (hDwmapi) {
        DwmSetWindowAttributeFunc pDwmSetWindowAttribute = 
            (DwmSetWindowAttributeFunc)GetProcAddress(hDwmapi, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            pDwmSetWindowAttribute(m_hwnd, 20, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
        }
        FreeLibrary(hDwmapi);
    }
}

void UI::SetProgress(int progress) {
    SendMessage(m_progressBar, PBM_SETPOS, progress, 0);
}

void UI::SetStatus(const std::wstring& status) {
    SetWindowTextW(m_statusLabel, status.c_str());
}

void UI::ShowError(const std::wstring& message) {
    MessageBoxW(m_hwnd, message.c_str(), L"Error", MB_OK | MB_ICONERROR);
}

void UI::ShowSuccess(const std::wstring& message) {
    MessageBoxW(m_hwnd, message.c_str(), L"Success", MB_OK | MB_ICONINFORMATION);
}

void UI::EnableControls(bool enable) {
    EnableWindow(m_installButton, enable);
    EnableWindow(m_cancelButton, !enable);
}

LRESULT CALLBACK UI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    UI* pUI = nullptr;
    
    if (uMsg == WM_CREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pUI = reinterpret_cast<UI*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pUI);
    } else {
        pUI = reinterpret_cast<UI*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    
    if (pUI) {
        return pUI->HandleMessage(uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT UI::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, COLOR_TEXT);
            SetBkColor(hdcStatic, COLOR_BG);
            return (LRESULT)m_bgBrush;
        }
        
        case WM_CTLCOLORBTN: {
            HDC hdcButton = (HDC)wParam;
            return (LRESULT)m_darkBrush;
        }
        
        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
            
            if (pDIS->CtlID == ID_INSTALL_BUTTON) {
                FillRect(pDIS->hDC, &pDIS->rcItem, CreateSolidBrush(COLOR_BUTTON));
                
                RECT textRect = pDIS->rcItem;
                SetTextColor(pDIS->hDC, RGB(255, 255, 255));
                SetBkMode(pDIS->hDC, TRANSPARENT);
                SelectObject(pDIS->hDC, m_fontButton);
                
                wchar_t text[256];
                GetWindowTextW(pDIS->hwndItem, text, 256);
                DrawTextW(pDIS->hDC, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                if (pDIS->itemState & ODS_FOCUS) {
                    InflateRect(&textRect, -4, -4);
                    DrawFocusRect(pDIS->hDC, &textRect);
                }
                
                return TRUE;
            }
            break;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_INSTALL_BUTTON) {
                if (m_installCallback) {
                    m_installCallback();
                }
                return 0;
            }
            if (LOWORD(wParam) == ID_CANCEL_BUTTON) {
                if (m_cancelCallback) {
                    m_cancelCallback();
                }
                return 0;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
            
            RECT rect;
            GetClientRect(m_hwnd, &rect);
            FillRect(hdc, &rect, m_bgBrush);
            
            EndPaint(m_hwnd, &ps);
            return 0;
        }
    }
    
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
