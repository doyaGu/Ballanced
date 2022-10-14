#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H
#ifdef WIN32
#include "ThreadSafeQueue.h"

template <class Item>
class BlockingQueue : public ThreadSafeQueue<Item>
{
public:
    BlockingQueue(long queueSize);
    virtual ~BlockingQueue();

    void Add(Item aItem);
    Item Remove();

private:
    // don't implement
    BlockingQueue(const BlockingQueue &aBlockingQueue);
    void operator=(BlockingQueue &aBlockingQueue);

    HANDLE mBlockOnFull;
    HANDLE mBlockOnEmpty;
};

template <class Item>
BlockingQueue<Item>::BlockingQueue(long queueSize)
    : ThreadSafeQueue<Item>()
{
    mBlockOnFull = CreateSemaphore(NULL, queueSize, queueSize, NULL);
    mBlockOnEmpty = CreateSemaphore(NULL, 0, queueSize, NULL);
}

template <class Item>
BlockingQueue<Item>::~BlockingQueue()
{
    CloseHandle(mBlockOnFull);
    CloseHandle(mBlockOnEmpty);
}

template <class Item>
void BlockingQueue<Item>::Add(Item aItem)
{
    WaitForSingleObject(mBlockOnFull, INFINITE);
    ThreadSafeQueue<Item>::Add(aItem);
    ReleaseSemaphore(mBlockOnEmpty, 1, NULL);
}

template <class Item>
Item BlockingQueue<Item>::Remove()
{
    WaitForSingleObject(mBlockOnEmpty, INFINITE);
    Item item = ThreadSafeQueue<Item>::Remove();
    ReleaseSemaphore(mBlockOnFull, 1, NULL);
    return item;
}
#endif // WIN32
#endif