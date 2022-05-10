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

    void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    void OnReturn(WPARAM wParam, LPARAM lParam);
    void OnSized();
    void OnClose();
    void OnPaint();
    void OnSetCursor();
    void OnMouseDoubleClick();
    void OnMouseClick();
    int OnKeyDown(UINT uKey);
    int OnSysKeyDown(UINT uKey);
    void OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnScreenModeChanged(WPARAM wParam, LPARAM lParam);
    void OnCommand(UINT id, UINT code);
    ~CGamePlayer();
    void Run();
    bool Step();
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Init(HINSTANCE hInstance, LPFNWNDPROC lpfnWndProc);
    void Play();
    void Pause();
    void Reset();
    void Done();
    bool LoadCMO(const char *filename);

    bool IsInitialized() const
    {
        return m_State == eInitialized;
    }

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