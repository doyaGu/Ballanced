#ifndef PLAYER_GAMEPLAYER_H
#define PLAYER_GAMEPLAYER_H

#include "Game.h"
#include "NeMoContext.h"
#include "WinContext.h"

class CGamePlayer
{
public:
    enum PlayerState
    {
        eInitial = 0,
        ePlaying = 1,
        ePaused = 2,
        eFocusLost = 3
    };

    static CGamePlayer &GetInstance();

    ~CGamePlayer();

    bool Init(HINSTANCE hInstance);
    void Run();
    bool Process();
    void Terminate();

    bool Load(const char *filename);

    void Play();
    void Pause();
    void Reset();

    int GetState() const
    {
        return m_State;
    }

    void OnDestroy();
    void OnMove();
    void OnSized();
    void OnPaint();
    void OnClose();
    void OnActivateApp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSetCursor();
    void OnGetMinMaxInfo(LPMINMAXINFO lpmmi);
    int OnKeyDown(UINT uKey);
    int OnSysKeyDown(UINT uKey);
    int OnCommand(UINT id, UINT code);
    void OnExceptionCMO(WPARAM wParam, LPARAM lParam);
    void OnReturn(WPARAM wParam, LPARAM lParam);
    bool OnLoadCMO(WPARAM wParam, LPARAM lParam);
    void OnExitToSystem(WPARAM wParam, LPARAM lParam);
    void OnExitToTitle(WPARAM wParam, LPARAM lParam);
    int OnChangeScreenMode(WPARAM wParam, LPARAM lParam);
    void OnGoFullscreen();
    void OnStopFullscreen();
    void OnSwitchFullscreen();

protected:
    int InitEngine();
    bool ReInitEngine();
    bool LoadRenderEngine();
    bool LoadPlugins();
    void RedirectLog();

    int m_State;
    CNeMoContext m_NeMoContext;
    CWinContext m_WinContext;
    CGame m_Game;

private:
    CGamePlayer();
    CGamePlayer(const CGamePlayer &);
    CGamePlayer &operator=(const CGamePlayer &);
};

#endif /* PLAYER_GAMEPLAYER_H */