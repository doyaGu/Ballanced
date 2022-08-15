#include "WinContext.h"

#include <string.h>

#include "ErrorProtocol.h"

#include "config.h"
#include "resource.h"

char RenderWindowName[8];

CWinContext::CWinContext()
    : m_MainWindow(NULL),
      m_RenderWindow(NULL),
      m_hInstance(NULL),
      m_hAccelTable(NULL),
      m_Width(DEFAULT_WIDTH),
      m_Height(DEFAULT_HEIGHT),
      m_Fullscreen(true),
      m_MainWndStyle(0),
      m_RenderWndStyle(0)
{
    memset(&m_RenderWndClass, 0, sizeof(WNDCLASSA));
    memset(&m_MainWndClass, 0, sizeof(WNDCLASSEXA));
    memset(m_MainWndName, 0, sizeof(m_MainWndName));
    memset(m_MainWndClassName, 0, sizeof(m_MainWndClassName));
    memset(m_RenderWndClassName, 0, sizeof(m_RenderWndClassName));
}

bool CWinContext::Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc, bool fullscreen, bool borderless, bool resizable)
{
    m_hInstance = hInstance;
    m_Fullscreen = fullscreen;
    m_Borderless = borderless;
    m_Resizable = resizable;

    LoadWindowNames();
    if (!RegisterWindowClasses(lpfnWndProc, m_Width, m_Height))
        return false;

    if (!CreateWindows())
        return false;

    return true;
}

void CWinContext::LoadWindowNames()
{
    ::LoadStringA(m_hInstance, IDS_MAIN_WND_NAME, m_MainWndName, 100);
    ::LoadStringA(m_hInstance, IDS_MAIN_WND_CLASS_NAME, m_MainWndClassName, 100);
    ::LoadStringA(m_hInstance, IDS_RENDER_WND_CLASS_NAME, m_RenderWndClassName, 100);
}

bool CWinContext::RegisterWindowClasses(LPFNWNDPROC lpfnWndProc, int width, int height)
{
    m_MainWndClass.lpfnWndProc = lpfnWndProc;
    m_MainWndClass.cbSize = sizeof(WNDCLASSEXA);
    m_MainWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    m_MainWndClass.cbClsExtra = 0;
    m_MainWndClass.cbWndExtra = 0;
    m_MainWndClass.hInstance = m_hInstance;
    m_MainWndClass.hIcon = ::LoadIconA(m_hInstance, (LPCSTR)IDI_PLAYER);
    m_MainWndClass.hCursor = ::LoadCursorA(NULL, (LPCSTR)IDC_ARROW);
    m_MainWndClass.hbrBackground = NULL;
    m_MainWndClass.lpszMenuName = NULL;
    m_MainWndClass.lpszClassName = m_MainWndClassName;
    m_MainWndClass.hIconSm = ::LoadIconA(m_hInstance, (LPCSTR)IDI_PLAYER);

    m_RenderWndClass.lpfnWndProc = lpfnWndProc;
    m_RenderWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    m_RenderWndClass.hInstance = m_hInstance;
    m_RenderWndClass.lpszClassName = m_RenderWndClassName;

    if (!::RegisterClassA(&m_RenderWndClass))
        return false;

    if (!::RegisterClassExA(&m_MainWndClass))
        return false;

    return true;
}

bool CWinContext::CreateWindows()
{
    m_MainWndStyle = (m_Fullscreen || m_Borderless) ? WS_POPUP : WS_OVERLAPPED | WS_CAPTION;
    if (!m_Fullscreen && m_Resizable)
        m_MainWndStyle |= WS_SIZEBOX;

    RECT rect = {0, 0, m_Width, m_Height};
    AdjustWindowRect(&rect, m_MainWndStyle, FALSE);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int x = (m_Fullscreen) ? CW_USEDEFAULT : (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int y = (m_Fullscreen) ? CW_USEDEFAULT : (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    m_MainWindow = ::CreateWindowExA(
        WS_EX_LEFT,
        m_MainWndClassName,
        m_MainWndName,
        m_MainWndStyle,
        x,
        y,
        width,
        height,
        NULL,
        NULL,
        m_hInstance,
        NULL);
    if (!m_MainWindow)
    {
        TT_ERROR("WinContext.cpp", "CWinContext::CreateWindows()", "MainWindow Create FAILED");
        return false;
    }

    m_RenderWndStyle = WS_CHILD | WS_VISIBLE;
    m_RenderWindow = ::CreateWindowExA(
        WS_EX_TOPMOST,
        m_RenderWndClassName,
        RenderWindowName,
        m_RenderWndStyle,
        0,
        0,
        m_Width,
        m_Height,
        m_MainWindow,
        NULL,
        m_hInstance,
        NULL);
    if (!m_RenderWindow)
    {
        TT_ERROR("WinContext.cpp", "CWinContext::CreateWindows()", "RenderWindow Create FAILED");
        return false;
    }

    m_hAccelTable = ::LoadAcceleratorsA(m_hInstance, (LPCSTR)IDR_ACCEL);

    return true;
}

void CWinContext::UpdateWindows()
{
    ::UpdateWindow(m_MainWindow);
    ::UpdateWindow(m_RenderWindow);
}

void CWinContext::ShowWindows()
{
    ::ShowWindow(m_MainWindow, SW_SHOW);
    ::ShowWindow(m_RenderWindow, SW_SHOW);
}

void CWinContext::RestoreWindow()
{
    ::ShowWindow(m_MainWindow, SW_RESTORE);
}

void CWinContext::MinimizeWindow()
{
    ::ShowWindow(m_MainWindow, SW_SHOWMINIMIZED);
}

void CWinContext::FocusMainWindow()
{
    ::SetFocus(m_MainWindow);
}

void CWinContext::FocusRenderWindow()
{
    ::SetFocus(m_RenderWindow);
}

void CWinContext::SetPosition(int x, int y)
{
    if (m_MainWindow && x + m_Width <= ::GetSystemMetrics(SM_CXSCREEN) && y + m_Height <= ::GetSystemMetrics(SM_CYSCREEN))
    {
        RECT rc;

        ::GetWindowRect(m_MainWindow, &rc);
        rc.right = x + (rc.right - rc.left);
        rc.bottom = y + (rc.bottom - rc.top);
        rc.left = x;
        rc.top = y;
        ::AdjustWindowRect(&rc, m_MainWndStyle, FALSE);
        if (!::SetWindowPos(m_MainWindow, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE))
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetPosition(...)", "wrong parameters");

        ::GetClientRect(m_MainWindow, &rc);
        if (!::SetWindowPos(m_RenderWindow, HWND_TOP, rc.left, rc.top, 0, 0, SWP_NOSIZE))
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetPosition(...)", "wrong parameters");
    }
}

void CWinContext::SetResolution(int width, int height)
{
    m_Width = width;
    m_Height = height;

    if (m_MainWindow)
    {
        RECT rect = {0, 0, width, height};
        ::AdjustWindowRect(&rect, m_MainWndStyle, FALSE);
        if (!::SetWindowPos(m_MainWindow, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE))
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetResolution(...)", "wrong parameters");

        if (!::SetWindowPos(m_RenderWindow, HWND_TOP, 0, 0, width, height, SWP_NOMOVE))
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetResolution(...)", "wrong parameters");
    }
}