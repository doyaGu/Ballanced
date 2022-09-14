#ifndef PLAYER_GAME_H
#define PLAYER_GAME_H

class CGameInfo;

class CGame
{
public:
    CGame();
    ~CGame();

    bool Load();
    bool FinishLoad();

    CGameInfo *NewGameInfo();
    CGameInfo *GetGameInfo() const;
    void SetGameInfo(CGameInfo *gameInfo);

    void SyncCamerasWithScreen();

private:
    CGameInfo *m_GameInfo;
};

#endif /* PLAYER_GAME_H */