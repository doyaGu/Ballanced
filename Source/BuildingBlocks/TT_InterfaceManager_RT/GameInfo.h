#ifndef BUILDINGBLOCKS_GAMEINFO_H
#define BUILDINGBLOCKS_GAMEINFO_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <string.h>
#include <windows.h>

class CGameInfo
{
public:
    CGameInfo() : next(NULL),
                  hkRoot(HKEY_CURRENT_USER),
                  gameScore(0),
                  levelScore(0),
                  type(0),
                  gameID(0),
                  levelID(0),
                  gameBonus(0),
                  levelBonus(0),
                  levelReached(0)
    {
        memset(gameName, 0, sizeof(gameName));
        memset(levelName, 0, sizeof(levelName));
        memset(fileName, 0, sizeof(fileName));
        memset(path, 0, sizeof(path));
        strcpy(regSubkey, "Software\\Terratools\\WorldMap\\Powerball");
    }

    CGameInfo(const char *path,
              int type,
              HKEY hkRoot,
              const char *regSubKey,
              const char *filename) : next(NULL),
                                      gameScore(0),
                                      levelScore(0),
                                      gameID(0),
                                      levelID(0),
                                      gameBonus(0),
                                      levelBonus(0),
                                      levelReached(0)
    {
        memset(gameName, 0, sizeof(gameName));
        memset(levelName, 0, sizeof(levelName));
        strcpy(this->path, path);
        this->type = type;
        this->hkRoot = hkRoot;
        strcpy(this->regSubkey, regSubKey);
        strcpy(this->fileName, filename);
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