#ifndef PLAYER_WINCONTEXT_H
#define PLAYER_WINCONTEXT_H

typedef LRESULT (CALLBACK *LPFNWNDPROC)(HWND, UINT, WPARAM, LPARAM);

class CWinContext
{
public:
    CWinContext();
    virtual ~CWinContext() {}

    bool Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc, bool fullscreen);

    void LoadWindowNames();
    void RegisterWindowClasses(LPFNWNDPROC lpfnWndProc, int width, int height);
    bool CreateWindows();
    void UpdateWindows();
    void ShowWindows();
    void MinimizeWindow();

    HINSTANCE GetAppInstance() const;
    HACCEL GetAccelTable() const;

    HWND GetMainWindow() const;
    HWND GetRenderWindow() const;

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
    bool m_IsWndClassRegistered;
    bool m_Fullscreen;
    DWORD m_MainWndStyle;
    DWORD m_RenderWndStyle;
};

#endif /* PLAYER_WINCONTEXT_H */