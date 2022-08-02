#ifndef PLAYER_GAMECONFIG_H
#define PLAYER_GAMECONFIG_H

struct CGameConfig
{
    int langId;
    int width;
    int height;
    int bpp;
    int driver;
    bool fullscreen;
    bool borderless;
    bool resizable;
    bool unlockFramerate;
    bool taskSwitchEnabled;
    bool pauseOnTaskSwitch;
    bool playerActive;
    bool godmode;
    bool debug;
    bool rookie;

    void LoadFromCmdline(int argc, char **argv);
    void LoadFromIni(const char *filename);
    void SaveToIni(const char *filename);
};

#endif // PLAYER_GAMECONFIG_H