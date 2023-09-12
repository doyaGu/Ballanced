#ifndef PLAYER_GAME_H
#define PLAYER_GAME_H

#include <exception>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include "CKAll.h"

class CGameInfo;
class CNeMoContext;

class CGameException : public std::exception
{
public:
    CGameException() : std::exception(), error(0) {}
    explicit CGameException(int err) : std::exception(), error(err) {}
    virtual ~CGameException();
    int error;
};

class CGame
{
public:
    CGame();
    virtual ~CGame();

    void Load();
    void Play();

    CGameInfo *NewGameInfo();
    CGameInfo *GetGameInfo();
    void SetGameInfo(CGameInfo *gameInfo);
    CNeMoContext *GetNeMoContext() const;
    void SetNeMoContext(CNeMoContext *nemoContext);

private:
    CKFileInfo m_CKFileInfo;
    char m_ProgPath[512];
    char m_FileName[512];
    CGameInfo *m_GameInfo;
    CNeMoContext *m_NeMoContext;
};

struct CGameData
{
    char fileName[128];
    char path[128];
    int type;
    HKEY hkRoot;
    char regSubkey[512];

    CGameData();
    CGameData(const char *filename, const char *path, const char *subkey, HKEY root = HKEY_CURRENT_USER);
    CGameData(const CGameData &rhs);
    CGameData &operator=(const CGameData &rhs);
};

class CGameDataManager
{
public:
    CGameDataManager();
    ~CGameDataManager() {}

    void Save(CGameInfo *gameInfo);
    void Load(CGameInfo *gameInfo, const char *filename);

private:
    int m_Count;
};

#endif /* PLAYER_GAME_H */