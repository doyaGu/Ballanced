// Player.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"

#include "GamePlayer.h"
#include "ErrorProtocol.h"
#include "LogProtocol.h"
#include "PlayerRegistry.h"

#include "TT_InterfaceManager_RT/GameInfo.h"
#include "ResDll.h"

RESOURCEMAP g_ResMap;

static void InitGameInfo(CGameInfo &gameInfo)
{
    strcpy(gameInfo.path, ".");
    gameInfo.type = 0;
    gameInfo.hkRoot = (HKEY)-1;
    strcpy(gameInfo.regSubkey, g_ResMap.settings);
    strcpy(gameInfo.fileName, "base.cmo");
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, g_ResMap.gameName);
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        return -1;
    }

    CGameInfo gameInfo;
    InitGameInfo(gameInfo);

    CGamePlayer player(&gameInfo, 1, true, hMutex, false);

    char buffer[512];
    ::LoadStringA(g_ResMap.hResDll, 2, buffer, 512);

    DWORD dwLangId = ::GetPrivateProfileIntA("Settings", "Language", -1, g_ResMap.pathSetting);
    if (dwLangId == -1)
    {
        if (::LoadStringA(g_ResMap.hResDll, 24, buffer, 512))
        {
            dwLangId = atoi(buffer);
        }
    }

    if (!player.IsInitialized())
    {
        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 6], buffer, 512);
        ::MessageBoxA(NULL, buffer, "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "GamePlayer Constructor - Abort");
        return -1;
    }

    CPlayerRegistry::GetInstance().Init(&player, hInstance);

    if (!player.IsInitialized())
    {
        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 6], buffer, 512);
        ::MessageBoxA(NULL, buffer, "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "The application could not be initialized!!!");
        player.Done();
        return -1;
    }

    if (!player.LoadCMO("base.cmo"))
    {
        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 7], buffer, 512);
        ::MessageBoxA(NULL, buffer, "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "SYSTEM HALTED");
        player.Done();
        return -1;
    }

    player.Run();
    return 0;
}
