#ifndef PLAYER_WINCONTEXT_H
#define PLAYER_WINCONTEXT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

typedef LRESULT(CALLBACK *LPFNWNDPROC)(HWND, UINT, WPARAM, LPARAM);

class CWinContext
{
public:
    CWinContext();
    ~CWinContext() {}

    bool Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc);

    bool Process();

    void UpdateWindows();

    void ShowMainWindow(bool show = true);
    void ShowRenderWindow(bool show = true);

    void FocusMainWindow();
    void FocusRenderWindow();

    void AdjustMainStyle(bool fullscreen);

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

    void GetPosition(int &x, int &y);
    void SetPosition(int x, int y);

    void GetMainSize(int &width, int &height);
    void GetRenderSize(int &width, int &height);

    void SetMainSize(int width, int height);
    void SetRenderSize(int width, int height);

private:
    CWinContext(const CWinContext &);
    CWinContext &operator=(const CWinContext &);

    void LoadWindowNames();
    bool RegisterWindowClasses(LPFNWNDPROC lpfnWndProc);
    bool CreateWindows();

    HWND m_MainWindow;
    HWND m_RenderWindow;
    HINSTANCE m_hInstance;
    HACCEL m_hAccelTable;
    WNDCLASSEXA m_MainWndClass;
    WNDCLASSA m_RenderWndClass;
    char m_MainWndName[128];
    char m_RenderWndName[128];
    char m_MainWndClassName[128];
    char m_RenderWndClassName[128];
    DWORD m_MainWndStyle;
    DWORD m_RenderWndStyle;
};

#endif /* PLAYER_WINCONTEXT_H */