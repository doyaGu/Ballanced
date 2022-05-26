#ifndef BUILDINGBLOCKS_GAMEINFO_H
#define BUILDINGBLOCKS_GAMEINFO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

class CGameInfo
{
public:
	CGameInfo() : next(NULL), hkRoot(HKEY_CURRENT_USER)
	{
		strcpy(regSubkey, "Software\\Terratools\\WorldMap\\Powerball");
	}
	
    virtual ~CGameInfo() {}

    CGameInfo *next;
    char gameName[128];
    char levelName[128];
    char fileName[128];
    char path[128];
    char regSubkey[512];
    HKEY hkRoot;
    int gameScore;
    int levelScore;
    int type;
    int gameID;
    int levelID;
    int gameBonus;
    int levelBonus;
    int levelReached;
};

#endif // BUILDINGBLOCKS_GAMEINFO_H