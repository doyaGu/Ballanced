#ifndef RESDLL_H
#define RESDLL_H

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <windows.h>

#include "resource.h" // main symbols

#ifdef RESDLL_INTERNAL
#define RESEXTERN extern __declspec(dllexport)
#else
#define RESEXTERN extern __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

const UINT RES_STR_ID[] = {
    51, 52, 53, 54,
    56, 57, 58, 59,
    61, 62, 63, 64,
    66, 67, 68, 69,
    71, 72, 73, 74,
    76, 77, 78, 79,
    81, 82, 83, 84,
    86, 87, 88, 89,
    91, 92, 93, 94,
    96, 97, 98, 99};

typedef struct tagRESOURCEMAP
{
    HKEY hkRoot;
    char gameName[16];
    char software[512];
    char configuration[40];
    char progName[16];
    char launcherDir[16];
    char launcherDirSetting[50];
    char fullScreen[20];
    char videoDriver[20];
    char videoMode[20];
    char dash8[40];
    char dash7[40];
    char dash10[16];
    char dash9[16];
    char dash5[128];
    char dash6[128];
    char componentFile[128];
    char components[128];
    char settings[128];
    char install[128];
    char launcher[50];
    char copyLauncher[50];
    char serverGamePath[16];
    DWORD dwDXVersion;
    char needDirectX[256];
    char unknownDirectXVersion[256];
    char needIntelPC[256];
    char needPentiumCPU[256];
    DWORD dwMinMem;
    char needMEM64MB[256];
    char launcherSetting[50];
    char copyLauncherSetting[50];
    char serverGamePathSetting[512];
    DWORD dwVideoModeSetting;
    DWORD dwVideoDriverSetting;
    BYTE fullScreenSetting;
    char progNameSetting[50];
    char pathSetting[257];
    DWORD fKey;
    HINSTANCE hResDll;
} RESOURCEMAP;

RESEXTERN void fillResourceMap(RESOURCEMAP *pResMap);

#ifdef __cplusplus
}
#endif

#endif /* RESDLL_H */