#include "StdAfx.h"

#include <stdio.h>
#include <stdlib.h>

#include "ResDll.h"

extern RESOURCEMAP g_ResMap;

static inline bool IsCDInserted(const char *path, const char *name)
{
    char achVolume[216] = "";
    DWORD dwVolumeSN;
    DWORD dwMaxComponentLength;
    DWORD fFileSystem;
    char achFileSystem[216] = "";

	return ::GetVolumeInformationA(
               path,
               achVolume,
               215,
               &dwVolumeSN,
               &dwMaxComponentLength,
               &fFileSystem,
               achFileSystem,
               215) &&
			::GetDriveTypeA(path) == DRIVE_CDROM &&
			!strcmp(achVolume, name);
}

bool AntiPiracyCheck()
{
    char achGameName[216] = "";
    HKEY hkResult;
    DWORD dwType, cbData;
    char achDrivePath[216] = "";
    DWORD dwDrives;
    char drive;

	if (::LoadStringA(g_ResMap.hResDll, 27, achGameName, 215) &&
		!::RegOpenKeyExA(g_ResMap.hkRoot, g_ResMap.settings, 0, KEY_READ, &hkResult) == ERROR_SUCCESS)
    {
        dwType = REG_SZ;
        cbData = 215;
		if (::RegQueryValueExA(hkResult, "SrcDisc", 0, (LPDWORD)&dwType, (LPBYTE)&achDrivePath, (LPDWORD)&cbData) != ERROR_SUCCESS)
        {
			::RegCloseKey(hkResult);
            return false;
        }
		::RegCloseKey(hkResult);

        if (IsCDInserted(achDrivePath, achGameName))
		{
            return true;
		}

		for (dwDrives = ::GetLogicalDrives(), drive = 'A';
             (dwDrives & 1) != 0;
             dwDrives >>= 1, ++drive)
        {
            sprintf(achDrivePath, "%c:", drive);
            if (IsCDInserted(achDrivePath, achGameName))
			{
                break;
			}
        }
        return dwDrives != 0;
    }
    return false;
}
