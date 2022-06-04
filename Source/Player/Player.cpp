#include "ErrorProtocol.h"
#include "GamePlayer.h"
#include "LogProtocol.h"
#include "PlayerRegistry.h"

#include "TT_InterfaceManager_RT/GameInfo.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, "Ballance");
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return -1;

    CGameInfo gameInfo(".", 0, (HKEY)-1, "Software\\Ballance\\Settings", "base.cmo");

    CGamePlayer player(&gameInfo, 1, hMutex);

    if (!player.IsInitialized())
    {
        ::MessageBoxA(NULL, "The game could not be started!", "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "GamePlayer Constructor - Abort");
        return -1;
    }

    CPlayerRegistry::GetInstance().Init(&player, hInstance);

    if (!player.IsInitialized())
    {
        ::MessageBoxA(NULL, "The game could not be started!", "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "The application could not be initialized!!!");
        player.Done();
        return -1;
    }

    if (!player.LoadCMO("base.cmo"))
    {
        ::MessageBoxA(NULL, "The base module could not be loaded!", "Error", MB_OK);
        TT_ERROR("Player.cpp", "WinMain()", "SYSTEM HALTED");
        player.Done();
        return -1;
    }

    player.Run();
    return 0;
}
