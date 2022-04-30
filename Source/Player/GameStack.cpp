#include "StdAfx.h"

#include "GameStack.h"

#include "ErrorProtocol.h"
#include "Game.h"

CGameStack::CGameStack(bool enable) : m_Enabled(enable), m_Node(new Node()), m_Count(0)
{
    m_Node->head = m_Node;
    m_Node->next = m_Node;
    m_Node->gameInfo = NULL;
}

CGameStack::~CGameStack()
{
    Node *pCur, *pDel;
    for (pCur = m_Node->head; pCur != m_Node; --m_Count)
    {
        pDel = pCur;
        pCur = pCur->head;
        pDel->next->head = pDel->next;
        pDel->head->next = pDel->next;
        delete pDel;
    }
    delete m_Node;
    m_Node = NULL;
    m_Count = 0;
}

void CGameStack::Push(CGameInfo *gameInfo)
{
    Node *pNode;
    Node *pHead;
    Node *pNodeAfterHead;
    Node *pNew;
    Node *pHeadForNew;
    Node *pNextForNew;
    CGameInfo **ppGameInfo = NULL;

    pNode = m_Node;
    pHead = pNode->head;
    pNodeAfterHead = pNode->head->next;
    pNew = new Node();
    pHeadForNew = pHead;
    if (!pHead)
    {
        pHeadForNew = pNew;
    }
    pNew->head = pHeadForNew;
    pNextForNew = pNodeAfterHead;
    if (!pNodeAfterHead)
    {
        pNextForNew = pNew;
    }
    pNew->next = pNextForNew;
    pHead->next = pNew;
    pNew->next->head = pNew;
    ppGameInfo = &pNew->gameInfo;
    if (ppGameInfo)
        *ppGameInfo = gameInfo;
    ++m_Count;
}

CGameInfo *CGameStack::RemoveHead()
{
    Node *pNode;
    Node *pHead;
    Node *pCur;
    CGameInfo *gameInfo = NULL;

    if (m_Count == 0)
    {
        TT_ERROR("GameStack.cpp", "CGameStack::RemoveHead()", "Game stack is empty");
        return NULL;
    }

    pNode = m_Node;
    pHead = pNode->head;
    gameInfo = pNode->head->gameInfo;
    while (pHead != pNode)
    {
        if (pHead->gameInfo == gameInfo)
        {
            pCur = pHead;
            pHead = pHead->head;
            pCur->next->head = pCur->head;
            pCur->head->next = pCur->next;
            delete pCur;
            --m_Count;
        }
        else
        {
            pHead = pHead->head;
        }
    }
    return gameInfo;
}

void CGameStack::ClearAll()
{
    CGameInfo *gameInfo;
    while (m_Count != 0)
    {
        gameInfo = RemoveHead();
        if (gameInfo)
            delete gameInfo;
    }
}

CGameInfo *CGameStack::GetGameInfo(const char *path)
{
    Node *pNode;
    Node *pHead;
    CGameInfo *gameInfo;

    pNode = m_Node;
    pHead = pNode->head;
    if (pNode->head == pNode)
    {
        return NULL;
    }

    while (true)
    {
        gameInfo = pHead->gameInfo;
        if (gameInfo)
        {
            if (!strcmp(gameInfo->fileName, path))
            {
                break;
            }
        }
        pHead = pHead->head;
        if (pHead == pNode)
        {
            return NULL;
        }
    }

    return pHead->gameInfo;
}