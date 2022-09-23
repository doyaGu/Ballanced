#define _WIN32_WINNT 0x0502
#include <windows.h>

#include "CmdlineParser.h"
#include "GameConfig.h"
#include "GamePlayer.h"
#include "Logger.h"
#include "Utils.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hMutex = ::CreateMutexA(NULL, FALSE, "Ballance");
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
        return -1;

    CGameConfig &config = CGameConfig::Get();

    CmdlineParser parser(__argc, __argv);

    // Locate config file
    {
        // First check that the config file is specified in command line
        config.LoadIniPathFromCmdline(parser);

        // If no config file is specified, use the default path
        if (!config.HasPath(eConfigPath))
        {
            char szPath[MAX_PATH];
            char drive[4];
            char dir[MAX_PATH];
            char filename[MAX_PATH];
            ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
            _splitpath(szPath, drive, dir, filename, NULL);
            sprintf(szPath, "%s%s%s.ini", drive, dir, filename);
            config.SetPath(eConfigPath, szPath);
        }
    }

    // Load paths in advance
    config.LoadPathsFromIni();
    config.LoadPathsFromCmdline(parser);

    // Check that the game root path is valid
    // The game directory should have a Bin directory which contains the Virtools Engine dlls
    {
        bool valid = false;
        char szPath[MAX_PATH];

        // If a config file is specified, check it first
        if (config.HasPath(eRootPath))
        {
            _snprintf(szPath, MAX_PATH, "%sBin\\", config.GetPath(eRootPath));
            if (utils::CheckVirtoolsDlls(szPath))
                valid = true;
        }
        else
        {
            // Otherwise, try the default path
            if (utils::GetDefaultRootPath(szPath, MAX_PATH))
            {
                config.SetPath(eRootPath, szPath);
                valid = true;
            }
        }

        if (!valid && utils::BrowsePath("Please specify the game root directory", szPath))
        {
            strcat(szPath, "\\");
            config.SetPath(eRootPath, szPath);
            _snprintf(szPath, MAX_PATH, "%sBin\\", config.GetPath(eRootPath));
            if (utils::CheckVirtoolsDlls(szPath))
                valid = true;
        }

        if (!valid)
        {
            ::MessageBoxA(NULL, "The player could not locate Virtools Engine dlls!", "Error", MB_OK);
            return -1;
        }
    }

    {
        char szPath[MAX_PATH];
        _snprintf(szPath, MAX_PATH, "%sBin\\", config.GetPath(eRootPath));
        // Some game scripts rely on the relative path, the current directory should keep the same with original player
        ::SetCurrentDirectoryA(szPath);
        // Add the path to delay-loaded DLLs
        ::SetDllDirectoryA(szPath);
    }

    // Load settings
    config.LoadFromIni();
    config.LoadFromCmdline(parser);

    CLogger::Get().Open("Player", config.GetPath(eRootPath), false);

    CGamePlayer player;
    if (!player.Init(hInstance, hMutex))
    {
        ::MessageBoxA(NULL, "The game could not be started!", "Error", MB_OK);
        CLogger::Get().Error("The application could not be initialized!!!");
        return -1;
    }

    if (!player.Load("base.cmo"))
    {
        ::MessageBoxA(NULL, "The base module could not be loaded!", "Error", MB_OK);
        CLogger::Get().Error("The base module could not be loaded!!!");
        return -1;
    }

    player.Run();
    player.Terminate();

    return 0;
}
