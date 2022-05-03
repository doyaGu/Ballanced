#include "StdAfx.h"

#include "WinContext.h"

#include <string.h>

#include "ErrorProtocol.h"

char RenderWindowName[8];

CWinContext::CWinContext()
    : m_MainWindow(NULL),
      m_RenderWindow(NULL),
      m_hInstance(NULL),
      m_hAccelTable(NULL),
      m_Width(DEFAULT_WIDTH),
      m_Height(DEFAULT_HEIGHT),
      m_IsWndClassRegistered(false),
      m_Fullscreen(true),
      m_MainWndX(0),
      m_MainWndY(0)
{
    memset(&m_RenderWndClass, 0, sizeof(WNDCLASSA));
    memset(&m_MainWndClass, 0, sizeof(WNDCLASSEXA));
    memset(m_MainWndName, 0, sizeof(m_MainWndName));
    memset(m_MainWndClassName, 0, sizeof(m_MainWndClassName));
    memset(m_RenderWndClassName, 0, sizeof(m_RenderWndClassName));
}

void CWinContext::Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc, bool fullscreen)
{
    m_hInstance = hInstance;
    m_Fullscreen = fullscreen;

    try
    {
        LoadWindowNames();
        RegisterWindowClasses(lpfnWndProc, m_Width, m_Height);
        if (m_IsWndClassRegistered)
        {
            CreateWindows();
            ShowWindows();
            UpdateWindows();
        }
    }
    catch (const CWinContextException &)
    {
        throw CWinContextException(3);
    }
    catch (...)
    {
        TT_ERROR("WinContext.cpp", "CWinContext::Init()", "Unhandled Exception");
    }
}

void CWinContext::LoadWindowNames()
{
    ::LoadStringA(m_hInstance, IDS_MAIN_WND_NAME, m_MainWndName, 100);
    ::LoadStringA(m_hInstance, IDS_MAIN_WND_CLASS_NAME, m_MainWndClassName, 100);
    ::LoadStringA(m_hInstance, IDS_RENDER_WND_CLASS_NAME, m_RenderWndClassName, 100);
}

void CWinContext::RegisterWindowClasses(LPFNWNDPROC lpfnWndProc, int width, int height)
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
    {
        m_IsWndClassRegistered = false;
        throw CWinContextException();
    }

    if (!::RegisterClassExA(&m_MainWndClass))
    {
        throw CWinContextException();
    }

    m_IsWndClassRegistered = true;
}

void CWinContext::CreateWindows()
{
    int mainWidth = m_Width + 2 * (::GetSystemMetrics(SM_CXFRAME));
    int mainHeight = m_Height + ::GetSystemMetrics(SM_CYCAPTION) + 2 * (::GetSystemMetrics(SM_CYFRAME));

    m_MainWndX = (::GetSystemMetrics(SM_CXSCREEN) - mainWidth) / 2;
    m_MainWndY = (::GetSystemMetrics(SM_CYSCREEN) - mainHeight) / 2;

    if (m_Fullscreen)
    {
        m_MainWindow = ::CreateWindowExA(
            WS_EX_LEFT,
            m_MainWndClassName,
            m_MainWndName,
            WS_POPUP,
            m_MainWndX,
            m_MainWndY,
            mainWidth,
            mainHeight,
            NULL,
            NULL,
            m_hInstance,
            NULL);
    }
    else
    {
        m_MainWindow = ::CreateWindowExA(
            WS_EX_LEFT,
            m_MainWndClassName,
            m_MainWndName,
            WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU),
            m_MainWndX,
            m_MainWndY,
            mainWidth,
            mainHeight,
            NULL,
            NULL,
            m_hInstance,
            NULL);
    }

    if (!m_MainWindow)
    {
        throw std::exception("MainWindow Create FAILED");
    }

    m_RenderWindow = ::CreateWindowExA(
        WS_EX_TOPMOST,
        m_RenderWndClassName,
        RenderWindowName,
        WS_CHILD | WS_VISIBLE,
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
        throw std::exception("RenderWindow Create FAILED");
    }

    m_hAccelTable = ::LoadAcceleratorsA(m_hInstance, (LPCSTR)IDR_ACCEL);
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

void CWinContext::MinimizeWindow()
{
    ::ShowWindow(m_RenderWindow, SW_SHOWMINIMIZED);
}

HINSTANCE CWinContext::GetAppInstance() const
{
    return m_hInstance;
}

HACCEL CWinContext::GetAccelTable() const
{
    return m_hAccelTable;
}

HWND CWinContext::GetMainWindow() const
{
    return m_MainWindow;
}

HWND CWinContext::GetRenderWindow() const
{
    return m_RenderWindow;
}

void CWinContext::SetResolution(int width, int height)
{
    m_Width = width;
    m_Height = height;

    int mainWidth = m_Width + 2 * (::GetSystemMetrics(SM_CXFRAME));
    int mainHeight = m_Height + ::GetSystemMetrics(SM_CYCAPTION) + 2 * (::GetSystemMetrics(SM_CYFRAME));

    if (m_MainWindow)
    {
        if (!::SetWindowPos(m_MainWindow, NULL, 0, 0, mainWidth, mainHeight, SWP_NOMOVE))
        {
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetResolution(...)", "wrong parameters");
            throw CWinContextException();
        }

        if (!::SetWindowPos(m_RenderWindow, NULL, 0, 0, m_Width, m_Height, SWP_NOMOVE))
        {
            TT_ERROR_BOX("WinContext.cpp", "CWinContext::SetResolution(...)", "wrong parameters");
            throw CWinContextException();
        }
    }
}
