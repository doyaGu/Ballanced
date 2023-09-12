#ifndef PLAYER_PLAYERREGISTRY_H
#define PLAYER_PLAYERREGISTRY_H

#include <exception>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

class CGamePlayer;

class CPlayerRegistryException : public std::exception {};

class CPlayerRegistry
{
public:
    static CPlayerRegistry &GetInstance();
    void Init(CGamePlayer *player, HINSTANCE hInstance);
    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    CPlayerRegistry();

    CPlayerRegistry(const CPlayerRegistry &);

    CPlayerRegistry &operator=(const CPlayerRegistry &);

    CGamePlayer *m_Player;
    bool m_Initialized;
};

#endif /* PLAYER_PLAYERREGISTRY_H */