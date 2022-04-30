#ifndef COMMON_GAMEINFO_H
#define COMMON_GAMEINFO_H

#include <windows.h>

class CGameInfo
{
public:
    CGameInfo();
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

#endif // COMMON_GAMEINFO_H