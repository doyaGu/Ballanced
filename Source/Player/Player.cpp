//#include <dsetup.h>

#include "ErrorProtocol.h"
#include "GamePlayer.h"
#include "LogProtocol.h"
#include "PlayerRegistry.h"

#include "ResDll.h"
#include "GameInfo.h"

RESOURCEMAP g_ResMap;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, "Ballance");
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return -1;

    CGameInfo gameInfo;
    strcpy(gameInfo.path, ".");
    gameInfo.type = 0;
    gameInfo.hkRoot = g_ResMap.hkRoot;
    strcpy(gameInfo.regSubkey, g_ResMap.settings);
	strcpy(gameInfo.fileName, "base.cmo");

    CGamePlayer player(&gameInfo, 1, true, hMutex, false);

    char buffer[512];
    HKEY hkSettings;
    DWORD dwLangId = -1;
    ::LoadStringA(g_ResMap.hResDll, 2, buffer, 512);
    if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, buffer, 0, KEY_QUERY_VALUE, &hkSettings) != ERROR_SUCCESS)
        if (::LoadStringA(g_ResMap.hResDll, 24, buffer, 512))
            dwLangId = atoi(buffer);

    DWORD dwRegValSize = sizeof(DWORD);
    DWORD dwRegValType = REG_DWORD;
    if (::RegQueryValueExA(hkSettings, "Language", 0, (LPDWORD)&dwRegValType, (LPBYTE)&dwLangId, (LPDWORD)&dwRegValSize) == ERROR_SUCCESS)
        ::RegCloseKey(hkSettings);

//    DWORD dwDXVersion;
//    DWORD dwDXMinorVersion;
//    if (!DirectXSetupGetVersion(&dwDXVersion, &dwDXMinorVersion))
//    {
//        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 1], buffer, 512);
//        ::MessageBoxA(NULL, buffer, "Error", MB_ICONERROR);
//        exit(-1);
//    }
//
//    if ((HIWORD(dwDXVersion) != 4 || LOWORD(dwDXVersion) <= 8) &&
//        (dwDXVersion != 8 || HIWORD(dwDXMinorVersion) != 0))
//    {
//        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8], buffer, 512);
//        ::MessageBoxA(NULL, buffer, "Attention", MB_OK);
//        exit(-1);
//    }

//    extern bool AntiPiracyCheck();
//    if (!AntiPiracyCheck())
//    {
//        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 5], buffer, 512);
//        ::MessageBoxA(NULL, buffer, "Attention", MB_OK);
//        ::ReleaseMutex(hMutex);
//        exit(-1);
//    }

    if (!player.IsInitialized())
    {
        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 6], buffer, 512);
        ::MessageBoxA(NULL, buffer, "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "GamePlayer Constructor - Abort");
        exit(-1);
    }

    try
    {
        CPlayerRegistry::GetInstance().Init(&player, hInstance);

        if (!player.IsInitialized())
        {
            ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 6], buffer, 512);
            ::MessageBoxA(NULL, buffer, "Error", MB_OK);
            TT_ERROR("Player.cpp", "WinMain()", "The application could not be initialized!!!");
            player.Done();
            exit(-1);
        }

        if (!player.LoadCMO("base.cmo"))
        {
            ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 7], buffer, 512);
            ::MessageBoxA(NULL, buffer, "Error", MB_OK);
            TT_ERROR("Player.cpp", "WinMain()", "SYSTEM HALTED");
            player.Done();
            exit(-1);
        }
    }
    catch (const CPlayerRegistryException &)
    {
        ::LoadStringA(g_ResMap.hResDll, RES_STR_ID[dwLangId * 8 + 6], buffer, 512);
        ::MessageBoxA(NULL, buffer, "Error_2", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "SYSTEM HALTED");
        player.Done();
        exit(-1);
    }

    player.Run();
    player.Done();

    return 0;
}
