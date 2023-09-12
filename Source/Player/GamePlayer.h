#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include <exception>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

#include "Game.h"
#include "GameStack.h"
#include "NeMoContext.h"
#include "WinContext.h"

class CPlayerRegistry;

class CGamePlayerException : public std::exception
{
public:
    CGamePlayerException() : std::exception(), error(0) {}
    explicit CGamePlayerException(int err) : std::exception(), error(err) {}
    int error;
};

class CGamePlayer
{
    friend CPlayerRegistry;

public:
    virtual void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    virtual void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    virtual bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    virtual void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    virtual void OnReturn(WPARAM wParam, LPARAM lParam);
    virtual void OnSized();
    virtual void OnClose();
    virtual void OnPaint();
    virtual void OnSetCursor();
    virtual void OnMouseDoubleClick();
    virtual void OnMouseClick();
    virtual int OnKeyDown(UINT uKey);
    virtual int OnSysKeyDown(UINT uKey);
    virtual void OnContextMenu();
    virtual void OnDropFiles(WPARAM wParam, LPARAM lParam);
    virtual int OnClickCaptionBar();
    virtual void OnCommand(UINT id, UINT code);

    CGamePlayer(CGameInfo *gameInfo, int n, bool defaultSetting, HANDLE hMutex, bool rookie);
    virtual ~CGamePlayer();
    virtual void Run();
    virtual bool Step();
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool IsInitialized() const;
    void Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc);
    void Done();
    bool LoadCMO(const char *filename);

private:
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    void Construct();
    bool InitEngine();
    bool ReInitEngine();
    void LoadEngineDLL();
    void LoadStdDLL();

    void RegisterGameInfo();

    bool m_Initialized;
    HANDLE m_hMutex;
    bool m_DefaultSetting;
    bool m_Cleared;
    char m_RenderPath[512];
    char m_PluginPath[512];
    char m_ManagerPath[512];
    char m_BehaviorPath[512];
    char m_Path[512];
    CGameDataManager m_DataManager;
    CNeMoContext m_NeMoContext;
    CWinContext m_WinContext;
    CGameStack m_Stack;
    CGame m_Game;
    bool m_IsRookie;
};

#endif /* PLAYER_GAMEPLAYER_H */