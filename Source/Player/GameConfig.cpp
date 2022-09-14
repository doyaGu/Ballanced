#include "GameConfig.h"

#include <io.h>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

#include "CmdlineParser.h"
#include "Utils.h"

static void ParseConfigsFromCmdline(CmdlineParser &parser, CGameConfig &config)
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
        if (parser.Next(arg, "--adaptive-camera", '\0'))
        {
            config.adaptiveCamera = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-widescreen", '\0'))
        {
            config.unlockWidescreen = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-high-resolution", '\0'))
        {
            config.unlockHighResolution = true;
            continue;
        }
        if (parser.Next(arg, "--skip-opening", '\0'))
        {
            config.skipOpening = true;
            continue;
        }
        if (parser.Next(arg, "--video-driver", 'v', 1))
        {
            if (arg.GetValue(0, value))
                config.driver = value;
            continue;
        }
        if (parser.Next(arg, "--bpp", 'b', 1))
        {
            if (arg.GetValue(0, value))
                config.bpp = value;
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
        if (parser.Next(arg, "--fullscreen", 'f'))
        {
            config.fullscreen = true;
            continue;
        }
        if (parser.Next(arg, "--unlock-framerate", 'u'))
        {
            config.unlockFramerate = true;
            continue;
        }
        if (parser.Next(arg, "--disable-filter", '\0'))
        {
            config.disableFilter = true;
            continue;
        }
        if (parser.Next(arg, "--disable-dithering", '\0'))
        {
            config.disableDithering = true;
            continue;
        }
        if (parser.Next(arg, "--antialias", '\0', 1))
        {
            if (arg.GetValue(0, value))
                config.antialias = value;
            continue;
        }
        if (parser.Next(arg, "--disable-mipmap", '\0'))
        {
            config.disableMipmap = true;
            continue;
        }
        if (parser.Next(arg, "--disable-specular", '\0'))
        {
            config.disableSpecular = true;
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
        if (parser.Next(arg, "--clip-mouse", '\0'))
        {
            config.clipMouse = true;
            continue;
        }
        if (parser.Next(arg, "--always-handle-input", '\0'))
        {
            config.alwaysHandleInput = true;
            continue;
        }
        if (parser.Next(arg, "--pause-on-deactivated", 'p'))
        {
            config.pauseOnDeactivated = true;
            continue;
        }
        if (parser.Next(arg, "--rookie", 'r'))
        {
            config.rookie = true;
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

static bool IniGetString(const char *section, const char *name, char *str, int size, const char *filename)
{
    return ::GetPrivateProfileStringA(section, name, "", str, size, filename) != 0;
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

CGameConfig &CGameConfig::Get()
{
    static CGameConfig config;
    return config;
}

void CGameConfig::SetPath(PathCategory category, const char *path)
{
    if (category < 0 || category >= ePathCategoryCount || !path)
        return;
    strncpy(m_Paths[category], path, MAX_PATH);
}

const char *CGameConfig::GetPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return NULL;
    return m_Paths[category];
}

bool CGameConfig::HasPath(PathCategory category) const
{
    if (category < 0 || category >= ePathCategoryCount)
        return NULL;
    return strcmp(m_Paths[category], "") != 0;
}

void CGameConfig::LoadFromCmdline(CmdlineParser &parser)
{
    ParseConfigsFromCmdline(parser, *this);
}

void CGameConfig::LoadIniPathFromCmdline(CmdlineParser &parser)
{
    CmdlineArg arg;
    std::string path;
    while (!parser.Done())
    {
        if (parser.Next(arg, "--config", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eConfigPath], sizeof(m_Paths[eConfigPath]));
            break;
        }
        parser.Skip();
    }
    parser.Reset();
}

void CGameConfig::LoadPathsFromCmdline(CmdlineParser &parser)
{
    CmdlineArg arg;
    std::string path;
    while (!parser.Done())
    {
        if (parser.Next(arg, "--root-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eRootPath], sizeof(m_Paths[eRootPath]));
            continue;
        }
        if (parser.Next(arg, "--plugins-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[ePluginPath], sizeof(m_Paths[ePluginPath]));
            continue;
        }
        if (parser.Next(arg, "--render-engines-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eRenderEnginePath], sizeof(m_Paths[eRenderEnginePath]));
            continue;
        }
        if (parser.Next(arg, "--managers-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eManagerPath], sizeof(m_Paths[eManagerPath]));
            continue;
        }
        if (parser.Next(arg, "--building-blocks-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eBuildingBlockPath], sizeof(m_Paths[eBuildingBlockPath]));
            continue;
        }
        if (parser.Next(arg, "--sound-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eSoundPath], sizeof(m_Paths[eSoundPath]));
            continue;
        }
        if (parser.Next(arg, "--bitmap-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eBitmapPath], sizeof(m_Paths[eBitmapPath]));
            continue;
        }
        if (parser.Next(arg, "--data-path", '\0', 1))
        {
            if (arg.GetValue(0, path))
                utils::GetAbsolutePath(path.c_str(), m_Paths[eDataPath], sizeof(m_Paths[eDataPath]));
            continue;
        }
        parser.Skip();
    }
    parser.Reset();
}

void CGameConfig::LoadFromIni(const char *filename)
{
    if (!filename)
        return;
    if (strcmp(filename, "") == 0)
    {
        if (strcmp(m_Paths[eConfigPath], "") != 0 && _access(m_Paths[eConfigPath], 0) != -1)
            filename = m_Paths[eConfigPath];
        else
            return;
    }

    IniGetInteger("Startup", "Language", langId, filename);
    IniGetBoolean("Startup", "AdaptiveCamera", adaptiveCamera, filename);
    IniGetBoolean("Startup", "UnlockWidescreen", unlockWidescreen, filename);
    IniGetBoolean("Startup", "UnlockHighResolution", unlockHighResolution, filename);
    IniGetBoolean("Startup", "SkipOpening", skipOpening, filename);

    IniGetInteger("Graphics", "Driver", driver, filename);
    IniGetInteger("Graphics", "BitsPerPixel", bpp, filename);
    IniGetInteger("Graphics", "Width", width, filename);
    IniGetInteger("Graphics", "Height", height, filename);
    IniGetBoolean("Graphics", "FullScreen", fullscreen, filename);
    IniGetBoolean("Graphics", "UnlockFramerate", unlockFramerate, filename);
    IniGetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    IniGetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    IniGetInteger("Graphics", "Antialias", antialias, filename);
    IniGetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    IniGetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);

    IniGetInteger("Window", "X", posX, filename);
    IniGetInteger("Window", "Y", posY, filename);
    IniGetBoolean("Window", "Borderless", borderless, filename);
    IniGetBoolean("Window", "Resizable", resizable, filename);
    IniGetBoolean("Window", "ClipMouse", clipMouse, filename);
    IniGetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    IniGetBoolean("Window", "PauseOnDeactivated", pauseOnDeactivated, filename);

    IniGetBoolean("Game", "Rookie", rookie, filename);
}

void CGameConfig::LoadPathsFromIni(const char *filename)
{
    if (!filename)
        return;
    if (strcmp(filename, "") == 0)
    {
        if (strcmp(m_Paths[eConfigPath], "") != 0 && _access(m_Paths[eConfigPath], 0) != -1)
            filename = m_Paths[eConfigPath];
        else
            return;
    }

    IniGetString("Path", "RootPath", m_Paths[eRootPath], MAX_PATH, filename);
    IniGetString("Path", "PluginPath", m_Paths[ePluginPath], MAX_PATH, filename);
    IniGetString("Path", "RenderEnginePath", m_Paths[eRenderEnginePath], MAX_PATH, filename);
    IniGetString("Path", "ManagerPath", m_Paths[eManagerPath], MAX_PATH, filename);
    IniGetString("Path", "BuildingBlockPath", m_Paths[eBuildingBlockPath], MAX_PATH, filename);
    IniGetString("Path", "SoundPath", m_Paths[eSoundPath], MAX_PATH, filename);
    IniGetString("Path", "BitmapPath", m_Paths[eBitmapPath], MAX_PATH, filename);
    IniGetString("Path", "DataPath", m_Paths[eDataPath], MAX_PATH, filename);
}

void CGameConfig::SaveToIni(const char *filename)
{
    if (!filename)
        return;
    if (strcmp(filename, "") == 0)
    {
        if (strcmp(m_Paths[eConfigPath], "") != 0 && _access(m_Paths[eConfigPath], 0) != -1)
            filename = m_Paths[eConfigPath];
        else
            return;
    }

    IniSetInteger("Startup", "Language", langId, filename);
    IniSetBoolean("Startup", "AdaptiveCamera", adaptiveCamera, filename);
    IniSetBoolean("Startup", "UnlockWidescreen", unlockWidescreen, filename);
    IniSetBoolean("Startup", "UnlockHighResolution", unlockHighResolution, filename);
    IniSetBoolean("Startup", "SkipOpening", skipOpening, filename);

    IniSetInteger("Graphics", "Driver", driver, filename);
    IniSetInteger("Graphics", "BitsPerPixel", bpp, filename);
    IniSetInteger("Graphics", "Width", width, filename);
    IniSetInteger("Graphics", "Height", height, filename);
    IniSetBoolean("Graphics", "FullScreen", fullscreen, filename);
    IniSetBoolean("Graphics", "UnlockFramerate", unlockFramerate, filename);
    IniSetBoolean("Graphics", "DisableFilter", disableFilter, filename);
    IniSetBoolean("Graphics", "DisableDithering", disableDithering, filename);
    IniSetInteger("Graphics", "Antialias", antialias, filename);
    IniSetBoolean("Graphics", "DisableMipmap", disableMipmap, filename);
    IniSetBoolean("Graphics", "DisableSpecular", disableSpecular, filename);

    IniSetInteger("Window", "X", posX, filename);
    IniSetInteger("Window", "Y", posY, filename);
    IniSetBoolean("Window", "Borderless", borderless, filename);
    IniSetBoolean("Window", "Resizable", resizable, filename);
    IniSetBoolean("Window", "ClipMouse", clipMouse, filename);
    IniSetBoolean("Window", "AlwaysHandleInput", alwaysHandleInput, filename);
    IniSetBoolean("Window", "PauseOnDeactivated", pauseOnDeactivated, filename);

    if (HasPath(eRootPath))
        IniSetString("Path", "RootPath", m_Paths[eRootPath], filename);
    if (HasPath(ePluginPath))
        IniSetString("Path", "PluginPath", m_Paths[ePluginPath], filename);
    if (HasPath(eRenderEnginePath))
        IniSetString("Path", "RenderEnginePath", m_Paths[eRenderEnginePath], filename);
    if (HasPath(eManagerPath))
        IniSetString("Path", "ManagerPath", m_Paths[eManagerPath], filename);
    if (HasPath(eBuildingBlockPath))
        IniSetString("Path", "BuildingBlockPath", m_Paths[eBuildingBlockPath], filename);
    if (HasPath(eSoundPath))
        IniSetString("Path", "SoundPath", m_Paths[eSoundPath], filename);
    if (HasPath(eBitmapPath))
        IniSetString("Path", "BitmapPath", m_Paths[eBitmapPath], filename);
    if (HasPath(eDataPath))
        IniSetString("Path", "DataPath", m_Paths[eDataPath], filename);
}