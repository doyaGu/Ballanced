#ifndef PLAYER_GAMESTACK_H
#define PLAYER_GAMESTACK_H

class CGameInfo;

class CGameStack
{
public:
    struct Node
    {
        Node *head;
        Node *next;
        CGameInfo *gameInfo;
    };

    CGameStack(bool enable);
    ~CGameStack();

    void Push(CGameInfo *gameInfo);
    CGameInfo *RemoveHead();
    void ClearAll();
    CGameInfo *GetGameInfo(const char *path);

private:
    bool m_Enabled;
    Node *m_Node;
    int m_Count;
};

#endif /* PLAYER_GAMESTACK_H */