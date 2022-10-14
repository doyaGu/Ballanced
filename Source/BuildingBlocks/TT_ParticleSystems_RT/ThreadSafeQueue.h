#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

// ACC - July 10, 2002
#include <list>

#ifdef WIN32
#include <windows.h>

using namespace std;

template <class Item>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue();
    virtual ~ThreadSafeQueue();

    void Add(const Item &aItem);
    Item Remove();
    bool IsEmpty();
    unsigned int NumItems();

private:
    // not implemented
    ThreadSafeQueue(const ThreadSafeQueue &aThreadSafeQueue);
    ThreadSafeQueue &operator=(const ThreadSafeQueue &aThreadSafeQueue);

    CRITICAL_SECTION mGuardAccess;
    list<Item> mQueue;
};

template <class Item>
ThreadSafeQueue<Item>::ThreadSafeQueue()
{
    InitializeCriticalSection(&mGuardAccess);
}

template <class Item>
ThreadSafeQueue<Item>::~ThreadSafeQueue()
{
    EnterCriticalSection(&mGuardAccess);
    mQueue.clear();
    LeaveCriticalSection(&mGuardAccess);
    DeleteCriticalSection(&mGuardAccess);
}

template <class Item>
void ThreadSafeQueue<Item>::Add(const Item &aItem)
{
    EnterCriticalSection(&mGuardAccess);
    mQueue.push_front(aItem);
    LeaveCriticalSection(&mGuardAccess);
}

template <class Item>
Item ThreadSafeQueue<Item>::Remove()
{
    EnterCriticalSection(&mGuardAccess);
    Item item;

    if (!mQueue.empty())
    {
        item = mQueue.back();
        mQueue.pop_back();
    }

    LeaveCriticalSection(&mGuardAccess);

    return item;
}

template <class Item>
bool ThreadSafeQueue<Item>::IsEmpty()
{

    EnterCriticalSection(&mGuardAccess);
    bool isempty = mQueue.empty();
    LeaveCriticalSection(&mGuardAccess);

    return isempty;
}

template <class Item>
unsigned int ThreadSafeQueue<Item>::NumItems()
{
    EnterCriticalSection(&mGuardAccess);
    unsigned int items = mQueue.size();
    LeaveCriticalSection(&mGuardAccess);

    return items;
}
#endif // WIN32

#endif // THREADSAFEQUEUE_H