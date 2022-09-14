#ifndef PLAYER_GAMECONFIG_H
#define PLAYER_GAMECONFIG_H

#include "config.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

class CmdlineParser;

enum PathCategory
{
    eConfigPath = 0,
    eRootPath = 1,
    ePluginPath = 2,
    eRenderEnginePath = 3,
    eManagerPath = 4,
    eBuildingBlockPath = 5,
    eSoundPath = 6,
    eBitmapPath = 7,
    eDataPath = 8,
    ePathCategoryCount = 9
};

struct CGameConfig
{
    // Startup Settings
    int langId;
    bool adaptiveCamera;
    bool unlockWidescreen;
    bool unlockHighResolution;
    bool skipOpening;

    // Graphics
    int driver;
    int bpp;
    int width;
    int height;
    bool fullscreen;
    bool unlockFramerate;

    bool disableFilter;
    bool disableDithering;
    int antialias;
    bool disableMipmap;
    bool disableSpecular;

    // Window Settings
    int posX;
    int posY;
    bool borderless;
    bool resizable;
    bool clipMouse;
    bool alwaysHandleInput;
    bool pauseOnDeactivated;

    // Game Settings
    bool rookie;

    // Paths

    static CGameConfig &Get();

    void SetPath(PathCategory category, const char *path);
    const char *GetPath(PathCategory category) const;
    bool HasPath(PathCategory category) const;

    void LoadFromCmdline(CmdlineParser &parser);
    void LoadIniPathFromCmdline(CmdlineParser &parser);
    void LoadPathsFromCmdline(CmdlineParser &parser);
    void LoadFromIni(const char *filename = "");
    void LoadPathsFromIni(const char *filename = "");
    void SaveToIni(const char *filename = "");

    CGameConfig()
        : langId(1),
          adaptiveCamera(false),
          unlockWidescreen(false),
          unlockHighResolution(false),
          skipOpening(false),
          driver(0),
          bpp(PLAYER_DEFAULT_BPP),
          width(PLAYER_DEFAULT_WIDTH),
          height(PLAYER_DEFAULT_HEIGHT),
          fullscreen(false),
          unlockFramerate(false),
          disableFilter(false),
          disableDithering(false),
          antialias(0),
          disableMipmap(false),
          disableSpecular(false),
          posX(2147483647),
          posY(2147483647),

          borderless(false),
          resizable(false),
          clipMouse(false),
          alwaysHandleInput(false),
          pauseOnDeactivated(false),
          rookie(false){};

private:
    char m_Paths[ePathCategoryCount][MAX_PATH];
};

#endif // PLAYER_GAMECONFIG_H