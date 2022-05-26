#include "StdAfx.h"

#include "GameStack.h"

#include "ErrorProtocol.h"
#include "Game.h"

#include "TT_InterfaceManager_RT/GameInfo.h"

CGameStack::CGameStack(bool enable) : m_Enabled(enable), m_Node(new Node()), m_Count(0)
{
    m_Node->head = m_Node;
    m_Node->next = m_Node;
    m_Node->gameInfo = NULL;
}

CGameStack::~CGameStack()
{
    Node *node, *ptr;
    for (node = m_Node->head; node != m_Node; --m_Count)
    {
        ptr = node;
        node = node->head;
        ptr->next->head = ptr->next;
        ptr->head->next = ptr->next;
        delete ptr;
    }
    delete m_Node;
    m_Node = NULL;
    m_Count = 0;
}

void CGameStack::Push(CGameInfo *gameInfo)
{
    Node *node;
    Node *head;
    Node *first;
    Node *newNode;
    Node *newHead;
    Node *nweNext;

    node = m_Node;
    head = node->head;
    first = node->head->next;
    newNode = new Node();
    newHead = head;
    if (!head)
    {
        newHead = newNode;
    }
    newNode->head = newHead;
    nweNext = first;
    if (!first)
    {
        nweNext = newNode;
    }
    newNode->next = nweNext;
    head->next = newNode;
    newNode->next->head = newNode;
    newNode->gameInfo = gameInfo;
    ++m_Count;
}

CGameInfo *CGameStack::RemoveHead()
{
    Node *node;
    Node *head;
    Node *ptr;
    CGameInfo *gameInfo = NULL;

    if (m_Count == 0)
    {
        TT_ERROR("GameStack.cpp", "CGameStack::RemoveHead()", "Game stack is empty");
        return NULL;
    }

    node = m_Node;
    head = node->head;
    gameInfo = node->head->gameInfo;
    while (head != node)
    {
        if (head->gameInfo == gameInfo)
        {
            ptr = head;
            head = head->head;
            ptr->next->head = ptr->head;
            ptr->head->next = ptr->next;
            delete ptr;
            --m_Count;
        }
        else
        {
            head = head->head;
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
    Node *node;
    Node *head;
    CGameInfo *gameInfo;

    node = m_Node;
    head = node->head;
    if (node->head == node)
    {
        return NULL;
    }

    while (true)
    {
        gameInfo = head->gameInfo;
        if (gameInfo)
        {
            if (!strcmp(gameInfo->fileName, path))
            {
                break;
            }
        }
        head = head->head;
        if (head == node)
        {
            return NULL;
        }
    }

    return head->gameInfo;
}