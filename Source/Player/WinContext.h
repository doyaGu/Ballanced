#ifndef PLAYER_WINCONTEXT_H
#define PLAYER_WINCONTEXT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

typedef LRESULT(CALLBACK *LPFNWNDPROC)(HWND, UINT, WPARAM, LPARAM);

class CWinContext
{
public:
    CWinContext();
    ~CWinContext() {}

    bool Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc, bool fullscreen = false, bool borderless = false, bool resizable = false);

    void LoadWindowNames();
    bool RegisterWindowClasses(LPFNWNDPROC lpfnWndProc, int width, int height);
    bool CreateWindows();
    void UpdateWindows();

    void ShowWindows();
    void RestoreWindow();
    void MinimizeWindow();

    void FocusMainWindow();
    void FocusRenderWindow();

    HINSTANCE GetAppInstance() const
    {
        return m_hInstance;
    }

    HACCEL GetAccelTable() const
    {
        return m_hAccelTable;
    }

    HWND GetMainWindow() const
    {
        return m_MainWindow;
    }

    HWND GetRenderWindow() const
    {
        return m_RenderWindow;
    }

    void SetPosition(int x, int y);

    void SetResolution(int width, int height);

private:
    CWinContext(const CWinContext &);
    CWinContext &operator=(const CWinContext &);

    WNDCLASSA m_RenderWndClass;
    WNDCLASSEXA m_MainWndClass;
    HWND m_MainWindow;
    HWND m_RenderWindow;
    HINSTANCE m_hInstance;
    HACCEL m_hAccelTable;
    char m_MainWndName[100];
    char m_MainWndClassName[100];
    char m_RenderWndClassName[100];
    int m_Height;
    int m_Width;
    bool m_Fullscreen;
    bool m_Borderless;
    bool m_Resizable;
    DWORD m_MainWndStyle;
    DWORD m_RenderWndStyle;
};

#endif /* PLAYER_WINCONTEXT_H */