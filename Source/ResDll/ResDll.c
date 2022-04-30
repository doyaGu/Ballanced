// ResDll.cpp : Defines the entry point for the DLL application.
//

#define RESDLL_INTERNAL
#include "ResDll.h"

#include <stdlib.h>

static RESOURCEMAP g_ResMap;

BOOL loadStrings(HINSTANCE hinstDLL)
{
#define LOAD_STRING(id, buf, size) LoadStringA(hinstDLL, id, buf, size)
#define LOAD_STRING_FAIL_RET(id, buf, size) \
	do                                      \
	{                                       \
		if (!LOAD_STRING(id, buf, size))    \
			return FALSE;                   \
	} while (0)

	char buffer[256];

	g_ResMap.hResDll = hinstDLL;

	LOAD_STRING_FAIL_RET(IDS_STRING1, g_ResMap.install, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING2, g_ResMap.settings, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING3, g_ResMap.components, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING4, g_ResMap.componentFile, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING5, g_ResMap.dash5, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING6, g_ResMap.dash6, 128);
	LOAD_STRING_FAIL_RET(IDS_STRING7, g_ResMap.dash7, 40);
	LOAD_STRING_FAIL_RET(IDS_STRING8, g_ResMap.dash8, 40);
	LOAD_STRING_FAIL_RET(IDS_STRING9, g_ResMap.dash9, 16);
	LOAD_STRING_FAIL_RET(IDS_STRING10, g_ResMap.dash10, 16);

	g_ResMap.hkRoot =
		(LOAD_STRING(IDS_STRING11, buffer, 20) && !strcmp(buffer, "HKEY_LOCAL_MACHINE"))
			? (HKEY)HKEY_LOCAL_MACHINE
			: (HKEY)0xFFFFFFFF;

	LOAD_STRING_FAIL_RET(IDS_STRING12, g_ResMap.launcher, 50);
	LOAD_STRING_FAIL_RET(IDS_STRING13, g_ResMap.copyLauncher, 50);
	LOAD_STRING_FAIL_RET(IDS_STRING14, g_ResMap.serverGamePath, 16);
	LOAD_STRING_FAIL_RET(IDS_STRING15, g_ResMap.launcherDir, 16);
	LOAD_STRING_FAIL_RET(IDS_STRING16, g_ResMap.progName, 16);
	LOAD_STRING_FAIL_RET(IDS_STRING17, g_ResMap.configuration, 40);
	LOAD_STRING_FAIL_RET(IDS_STRING18, g_ResMap.software, 40);
	LOAD_STRING_FAIL_RET(IDS_STRING19, g_ResMap.gameName, 16);
	LOAD_STRING_FAIL_RET(IDS_STRING20, g_ResMap.videoMode, 20);
	LOAD_STRING_FAIL_RET(IDS_STRING21, g_ResMap.videoDriver, 20);
	LOAD_STRING_FAIL_RET(IDS_STRING22, g_ResMap.fullScreen, 20);

	g_ResMap.dwDXVersion =
		(LOAD_STRING(IDS_STRING50, buffer, 256))
			? atoi(buffer)
			: 8;

	LOAD_STRING_FAIL_RET(IDS_STRING51, g_ResMap.needDirectX, 256);
	LOAD_STRING_FAIL_RET(IDS_STRING52, g_ResMap.unknownDirectXVersion, 256);
	LOAD_STRING_FAIL_RET(IDS_STRING53, g_ResMap.needIntelPC, 256);
	LOAD_STRING_FAIL_RET(IDS_STRING54, g_ResMap.needPentiumCPU, 256);

	g_ResMap.dwMinMem =
	(LOAD_STRING(IDS_STRING55, buffer, 256))
		? atoi(buffer)
		: 64000000;

	LOAD_STRING_FAIL_RET(IDS_STRING56, g_ResMap.needMEM64MB, 256);

#undef LOAD_STRING
#undef LOAD_STRING_FAIL_RET

	return TRUE;
}

DWORD loadSettings()
{
	HKEY hKey;
	DWORD dwType, dwSize;


#define REG_OPEN_KEY(root, subkey) RegOpenKeyExA(root, subkey, 0, KEY_READ, &hKey)
#define REG_QUERY_VALUE(name, type, buf, size) (dwType = type, dwSize = size, RegQueryValueExA(hKey, name, NULL, &dwType, (LPBYTE)(buf), &dwSize))
#define REG_CLOSE_KEY() RegCloseKey(hKey);

	if (g_ResMap.hkRoot == (HKEY)0xFFFFFFFF)
	{
		MessageBoxA(NULL, "RegKey was not defined in the resource", "Error", MB_OK);
		return FALSE;
	}

	if (REG_OPEN_KEY(g_ResMap.hkRoot, g_ResMap.install) != ERROR_SUCCESS)
	{
		REG_CLOSE_KEY();
		g_ResMap.noKey |= 0x01;
	}
	else
	{
		if (REG_QUERY_VALUE(g_ResMap.launcher, REG_SZ, g_ResMap.launcherSetting, 50) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		/// UNREFERENCED_CODE
		// if (REG_QUERY_VALUE(g_ResMap.ComponentFile, REG_SZ, g_ResMap.ComponentFileSetting, 512) != ERROR_SUCCESS)
		//	g_ResMap.noKey |= 0x01;

		if (REG_QUERY_VALUE("Path", REG_SZ, g_ResMap.pathSetting, 512) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		/// UNREFERENCED_CODE
		// if (REG_QUERY_VALUE("ResourceFile", REG_SZ, g_ResMap.pathSetting, 64) != ERROR_SUCCESS)
		//	g_ResMap.noKey |= 0x01;

		if (REG_QUERY_VALUE(g_ResMap.copyLauncher, REG_SZ, g_ResMap.copyLauncherSetting, 50) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		if (REG_QUERY_VALUE(g_ResMap.serverGamePath, REG_SZ, g_ResMap.serverGamePathSetting, 512) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		if (REG_QUERY_VALUE(g_ResMap.launcherDir, REG_SZ, g_ResMap.launcherDirSetting, 50) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		if (REG_QUERY_VALUE(g_ResMap.progName, REG_SZ, g_ResMap.progNameSetting, 50) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x01;

		REG_CLOSE_KEY();
	}

	if (REG_OPEN_KEY(g_ResMap.hkRoot, g_ResMap.settings) != ERROR_SUCCESS)
	{
		REG_CLOSE_KEY();
		g_ResMap.noKey |= 0x02;
		g_ResMap.dwVideoModeSetting = 0x3C0000;
		g_ResMap.dwVideoDriverSetting = 0;
		g_ResMap.fullScreenSetting = 0;
	}
	else
	{
		if (REG_QUERY_VALUE(g_ResMap.videoDriver, REG_DWORD, &g_ResMap.dwVideoDriverSetting, 4) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x02;
		if (REG_QUERY_VALUE(g_ResMap.videoMode, REG_DWORD, &g_ResMap.dwVideoModeSetting, 4) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x02;
		if (REG_QUERY_VALUE(g_ResMap.fullScreen, REG_DWORD, &g_ResMap.fullScreenSetting, 4) != ERROR_SUCCESS)
			g_ResMap.noKey |= 0x02;

		REG_CLOSE_KEY();
	}

#undef REG_OPEN_KEY
#undef REG_QUERY_VALUE
#undef REG_CLOSE_KEY

	return g_ResMap.noKey;
}

BOOL init(HINSTANCE hinstDLL)
{
	if (loadStrings(hinstDLL))
		return loadSettings() != 0;
	return FALSE;
}

void fillResourceMap(RESOURCEMAP *pResMap)
{
	if (g_ResMap.hkRoot == (HKEY)0xFFFFFFFF)
		return;

    pResMap->hResDll = g_ResMap.hResDll;
    pResMap->hkRoot = g_ResMap.hkRoot;
    strcpy(pResMap->software, g_ResMap.software);
    strcpy(pResMap->components, g_ResMap.components);
    strcpy(pResMap->settings, g_ResMap.settings);
    strcpy(pResMap->install, g_ResMap.install);
    strcpy(pResMap->progName, g_ResMap.progName);
    strcpy(pResMap->launcherDir, g_ResMap.launcherDir);
    strcpy(pResMap->componentFile, g_ResMap.componentFile);
    strcpy(pResMap->launcher, g_ResMap.launcher);
    strcpy(pResMap->copyLauncher, g_ResMap.copyLauncher);
    strcpy(pResMap->serverGamePath, g_ResMap.serverGamePath);
    strcpy(pResMap->fullScreen, g_ResMap.fullScreen);
    strcpy(pResMap->videoMode, g_ResMap.videoMode);
    strcpy(pResMap->videoDriver, g_ResMap.videoDriver);
    strcpy(pResMap->configuration, g_ResMap.configuration);
    strcpy(pResMap->dash8, g_ResMap.dash8);
    strcpy(pResMap->dash7, g_ResMap.dash7);
    strcpy(pResMap->dash10, g_ResMap.dash10);
    strcpy(pResMap->dash9, g_ResMap.dash9);
    strcpy(pResMap->dash5, g_ResMap.dash5);
    strcpy(pResMap->dash6, g_ResMap.dash6);
    strcpy(pResMap->gameName, g_ResMap.gameName);
    strcpy(pResMap->launcherSetting, g_ResMap.launcherSetting);
    strcpy(pResMap->launcherDirSetting, g_ResMap.launcherDirSetting);
    strcpy(pResMap->copyLauncherSetting, g_ResMap.copyLauncherSetting);
    strcpy(pResMap->serverGamePathSetting, g_ResMap.serverGamePathSetting);
    strcpy(pResMap->progNameSetting, g_ResMap.progNameSetting);
    strcpy(pResMap->pathSetting, g_ResMap.pathSetting);
    pResMap->fullScreenSetting = g_ResMap.fullScreenSetting;
	pResMap->dwVideoModeSetting = g_ResMap.dwVideoModeSetting;
    pResMap->dwVideoDriverSetting = g_ResMap.dwVideoDriverSetting;
	pResMap->noKey = g_ResMap.noKey;
	pResMap->dwDXVersion = g_ResMap.dwDXVersion;
    strcpy(pResMap->needDirectX, g_ResMap.needDirectX);
    strcpy(pResMap->unknownDirectXVersion, g_ResMap.unknownDirectXVersion);
    strcpy(pResMap->needIntelPC, g_ResMap.needIntelPC);
    strcpy(pResMap->needPentiumCPU, g_ResMap.needPentiumCPU);
    pResMap->dwMinMem = g_ResMap.dwMinMem;
    strcpy(pResMap->needMEM64MB, g_ResMap.needMEM64MB);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	return init(hinstDLL);
}
