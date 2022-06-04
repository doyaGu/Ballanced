#include "GameStack.h"

#include "ErrorProtocol.h"
#include "Game.h"

#include "TT_InterfaceManager_RT/GameInfo.h"

CGameStack::CGameStack() : m_Node(new Node), m_Count(0)
{
    m_Node->head = m_Node;
    m_Node->next = m_Node;
    m_Node->gameInfo = NULL;
}

CGameStack::~CGameStack()
{
    Node *ptr;
    for (Node *node = m_Node->head; node != m_Node; --m_Count)
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
    if (!gameInfo)
        return;

    Node *node = m_Node;
    Node *head = node->head;
    Node *first = node->head->next;

    Node *newNode = new Node;
    Node *newHead = head;
    if (!head)
        newHead = newNode;

    newNode->head = newHead;
    Node *newNext = first;
    if (!first)
        newNext = newNode;

    newNode->next = newNext;
    newHead->next = newNode;
    newNode->next->head = newNode;
    newNode->gameInfo = gameInfo;
    ++m_Count;
}

CGameInfo *CGameStack::RemoveHead()
{
    if (m_Count == 0)
    {
        TT_ERROR("GameStack.cpp", "CGameStack::RemoveHead()", "Game stack is empty");
        return NULL;
    }

    Node *node = m_Node;
    Node *head = node->head;
    CGameInfo *gameInfo = node->head->gameInfo;
    while (head != node)
    {
        if (head->gameInfo == gameInfo)
        {
            Node *ptr = head;
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
    Node *node = m_Node;
    Node *head = node->head;
    if (node->head == node)
        return NULL;

    CGameInfo *gameInfo;
    while (true)
    {
        gameInfo = head->gameInfo;
        if (gameInfo && !strcmp(gameInfo->fileName, path))
            break;

        head = head->head;
        if (head == node)
            return NULL;
    }

    return head->gameInfo;
}