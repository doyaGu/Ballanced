#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

#include "Game.h"
#include "GameConfig.h"
#include "GameStack.h"
#include "NeMoContext.h"
#include "WinContext.h"

#include "config.h"

class CPlayerRegistry;

class CGamePlayer
{
    friend CPlayerRegistry;

public:
    enum PlayerState
    {
        eInitial = 0,
        eInitialized = 1,
    };

    CGamePlayer(CGameInfo *gameInfo, int n, HANDLE hMutex);
    ~CGamePlayer();

    void Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc);
    void Run();
    bool Step();
    void Done();
    bool LoadCMO(const char *filename);
    bool IsInitialized() const
    {
        return m_State == eInitialized;
    }

    void OnDestroy();
    void OnMove(int x, int y);
    void OnSized();
    void OnPaint();
    void OnClose();
    LRESULT OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnKeyDown(UINT uKey);
    int OnSysKeyDown(UINT uKey);
    void OnCommand(UINT id, UINT code);
    void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    void OnReturn(WPARAM wParam, LPARAM lParam);
    bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    LRESULT OnChangeScreenMode(WPARAM wParam, LPARAM lParam);
    void OnGoFullscreen();
    void OnStopFullscreen();

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    void Construct();
    int InitEngine();
    bool ReInitEngine();
    bool LoadEngineDLL();
    bool LoadStdDLL();

    void RegisterGameInfo();

    int m_State;
    HANDLE m_hMutex;
    bool m_Cleared;
    char m_RenderPath[512];
    char m_PluginPath[512];
    char m_ManagerPath[512];
    char m_BehaviorPath[512];
    char m_Path[512];
    char m_IniPath[512];
    CGameDataManager m_DataManager;
    CNeMoContext m_NeMoContext;
    CWinContext m_WinContext;
    CGameStack m_Stack;
    CGameConfig m_Config;
    CGame m_Game;
};

#endif /* PLAYER_GAMEPLAYER_H */