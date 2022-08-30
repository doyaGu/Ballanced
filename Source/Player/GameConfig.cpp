#include "GameConfig.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

#include "CmdlineParser.h"

static void ParseCmdline(CmdlineParser &parser, CGameConfig &config)
{
    CmdlineArg arg;
    long value = 0;

    while (!parser.Done())
    {
        if (parser.Next(arg, "--lang", 'l'))
        {
            if (arg.GetValue(0, value))
                config.langId = value;
            continue;
        }
        if (parser.Next(arg, "--position-x", 'x', 1))
        {
            if (arg.GetValue(0, value))
                config.posX = value;
            continue;
        }
        if (parser.Next(arg, "--position-y", 'y', 1))
        {
            if (arg.GetValue(0, value))
                config.posY = value;
            continue;
        }
        if (parser.Next(arg, "--width", 'w', 1))
        {
            if (arg.GetValue(0, value))
                config.width = value;
            continue;
        }
        if (parser.Next(arg, "--height", 'h', 1))
        {
            if (arg.GetValue(0, value))
                config.height = value;
            continue;
        }
        if (parser.Next(arg, "--bpp", 'b', 1))
        {
            if (arg.GetValue(0, value))
                config.bpp = value;
            continue;
        }
        if (parser.Next(arg, "--videodriver", 'v', 1))
        {
            if (arg.GetValue(0, value))
                config.driver = value;
            continue;
        }
        if (parser.Next(arg, "--fullscreen", 'f'))
        {
            config.fullscreen = true;
            continue;
        }
        if (parser.Next(arg, "--borderless", 'c'))
        {
            config.borderless = true;
            continue;
        }
        if (parser.Next(arg, "--resizable", 's'))
        {
            config.resizable = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-framerate", 'u'))
        {
            config.unlockFramerate = true;
            continue;
        }
        if (parser.Next(arg, "--disable-task-switch", 'e'))
        {
            config.taskSwitchEnabled = false;
            continue;
        }
        if (parser.Next(arg, "--pause-on-task-switch", 'p'))
        {
            config.taskSwitchEnabled = true;
            config.pauseOnTaskSwitch = true;
            continue;
        }
        if (parser.Next(arg, "--player-active", 'a'))
        {
            config.playerActive = true;
            continue;
        }
        if (parser.Next(arg, "--god-mode", 'g'))
        {
            config.godmode = true;
            continue;
        }
        if (parser.Next(arg, "--debug", 'd'))
        {
            config.debug = true;
            continue;
        }
        if (parser.Next(arg, "--rookie", 'r'))
        {
            config.rookie = true;
            continue;
        }
        parser.Skip();
    }
}

static bool IniGetString(const char *section, const char *name, std::string &str, const char *filename)
{
    char buf[512];
    ::GetPrivateProfileStringA(section, name, "", buf, 512, filename);
    if (strcmp(buf, "") == 0)
        return false;
    str = buf;
    return true;
}

static bool IniGetInteger(const char *section, const char *name, int &value, const char *filename)
{
    char buf[512];
    ::GetPrivateProfileStringA(section, name, "", buf, 512, filename);
    if (strcmp(buf, "") == 0)
        return false;
    int val = atoi(buf);
    if (val == 0 && strcmp(buf, "0") != 0)
        return false;
    value = val;
    return true;
}

static bool IniGetBoolean(const char *section, const char *name, bool &value, const char *filename)
{
    UINT val = ::GetPrivateProfileIntA(section, name, -1, filename);
    if (val == -1)
        return false;
    value = (val == 1);
    return true;
}

static bool IniGetResolution(int &width, int &height, const char *filename)
{
    int videoMode = 0;
    if (!IniGetInteger("Settings", "VideoMode", videoMode, filename))
        return false;
    width = HIWORD(videoMode);
    height = LOWORD(videoMode);
    return true;
}

static bool IniGetBppAndDriver(int &bpp, int &driver, const char *filename)
{
    int videoDriver = 0;
    if (!IniGetInteger("Settings", "VideoDriver", videoDriver, filename))
        return false;
    bpp = (videoDriver & 1) != 0 ? 32 : 16;
    driver = videoDriver >> 16;
    return true;
}

static bool IniSetString(const char *section, const char *name, const char *str, const char *filename)
{
    return ::WritePrivateProfileStringA(section, name, str, filename) != 0;
}

static bool IniSetInteger(const char *section, const char *name, int value, const char *filename)
{
    char buf[64];
    sprintf(buf, "%d", value);
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetBoolean(const char *section, const char *name, bool value, const char *filename)
{
    const char *buf = (value) ? "1" : "0";
    return ::WritePrivateProfileStringA(section, name, buf, filename) != 0;
}

static bool IniSetResolution(int width, int height, const char *filename)
{
    return IniSetInteger("Settings", "VideoMode", (width << 16) | height, filename);
}

static bool IniSetBppAndDriver(int bpp, int driver, const char *filename)
{
    int videoDriver = 0;
    if (bpp == 32)
        videoDriver = (driver << 16) | 1;
    else if (bpp == 16)
        videoDriver = (driver << 16) | 0;
    return IniSetInteger("Settings", "VideoDriver", videoDriver, filename);
}

void CGameConfig::LoadFromCmdline(int argc, char **argv)
{
    CmdlineParser parser(argc, argv);
    ParseCmdline(parser, *this);
}

void CGameConfig::LoadFromIni(const char *filename)
{
    IniGetInteger("Settings", "Language", langId, filename);
    IniGetInteger("Settings", "X", posX, filename);
    IniGetInteger("Settings", "Y", posY, filename);
    IniGetResolution(width, height, filename);
    IniGetBppAndDriver(bpp, driver, filename);
    IniGetBoolean("Settings", "FullScreen", fullscreen, filename);
    IniGetBoolean("Settings", "Borderless", borderless, filename);
    IniGetBoolean("Settings", "Resizable", resizable, filename);
    IniGetBoolean("Settings", "UnlockFramerate", unlockFramerate, filename);
    IniGetBoolean("Settings", "TaskSwitchEnabled", taskSwitchEnabled, filename);
    IniGetBoolean("Settings", "PauseOnTaskSwitch", pauseOnTaskSwitch, filename);
    if (pauseOnTaskSwitch) taskSwitchEnabled = true;
    IniGetBoolean("Settings", "PlayerActive", playerActive, filename);
    IniGetBoolean("Settings", "GodMode", godmode, filename);
    IniGetBoolean("Settings", "Debug", debug, filename);
    IniGetBoolean("Settings", "Rookie", rookie, filename);
}

void CGameConfig::SaveToIni(const char *filename)
{
    IniSetInteger("Settings", "Language", langId, filename);
    if (!fullscreen) {
        IniSetInteger("Settings", "X", posX, filename);
        IniSetInteger("Settings", "Y", posY, filename);
    }
    IniSetResolution(width, height, filename);
    IniSetBppAndDriver(bpp, driver, filename);
    IniSetBoolean("Settings", "FullScreen", fullscreen, filename);
    IniSetBoolean("Settings", "Borderless", borderless, filename);
    IniSetBoolean("Settings", "Resizable", resizable, filename);
    IniSetBoolean("Settings", "UnlockFramerate", unlockFramerate, filename);
    IniSetBoolean("Settings", "TaskSwitchEnabled", taskSwitchEnabled, filename);
    IniSetBoolean("Settings", "PauseOnTaskSwitch", pauseOnTaskSwitch, filename);
    // IniSetBoolean("Settings", "PlayerActive", playerActive, filename);
    // IniSetBoolean("Settings", "GodMode", godmode, filename);
    // IniSetBoolean("Settings", "Debug", debug, filename);
    // IniSetBoolean("Settings", "Rookie", rookie, filename);
}