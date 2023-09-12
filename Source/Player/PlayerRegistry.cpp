#include "PlayerRegistry.h"

#include "ErrorProtocol.h"
#include "GamePlayer.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CPlayerRegistry::GetInstance().WndProc(hWnd, uMsg, wParam, lParam);
}

CPlayerRegistry &CPlayerRegistry::GetInstance()
{
    static CPlayerRegistry pr;
    return pr;
}

void CPlayerRegistry::Init(CGamePlayer *player, HINSTANCE hInstance)
{
    m_Player = player;
    player->Init(hInstance, ::WndProc);
    if (player->IsInitialized())
        m_Initialized = true;
}

LRESULT CPlayerRegistry::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_Player)
    {
        TT_ERROR("PayerRegistry.cpp", "CPlayerRegistry::WndProc()", "Couldn't register gameplayer");
        ::PostQuitMessage(-1);
        return ::DefWindowProcA(hWnd, uMsg, wParam, lParam);
    }

    return m_Player->WndProc(hWnd, uMsg, wParam, lParam);
}

CPlayerRegistry::CPlayerRegistry() : m_Initialized(false) {}