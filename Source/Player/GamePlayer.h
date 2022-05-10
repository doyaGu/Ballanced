#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "Game.h"
#include "NeMoContext.h"
#include "WinContext.h"
#include "GameStack.h"

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

    CGamePlayer(CGameInfo *gameInfo, int n, bool defaultSetting, HANDLE hMutex, bool rookie);
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
    void OnSized();
    void OnPaint();
    void OnClose();
    void OnActivateApp(WPARAM wParam, LPARAM lParam);
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnKeyDown(UINT uKey);
    int OnSysKeyDown(UINT uKey);
    void OnCommand(UINT id, UINT code);
    void OnMouseClick(UINT uMsg);
    void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    void OnReturn(WPARAM wParam, LPARAM lParam);
    bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    void OnScreenModeChanged(WPARAM wParam, LPARAM lParam);
    void OnGoFullscreen();
    void OnStopFullscreen();

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);

    void Construct();
    bool InitEngine();
    bool ReInitEngine();
    bool LoadEngineDLL();
    bool LoadStdDLL();

    bool RegisterGameInfoToInterfaceManager();

    int m_State;
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
    bool m_TaskSwitchEnabled;
    bool m_PauseOnTaskSwitch;
};

#endif /* PLAYER_GAMEPLAYER_H */