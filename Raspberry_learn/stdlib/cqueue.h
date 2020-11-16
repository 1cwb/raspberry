#ifndef CQUEUE_H
#define CQUEUE_H
#include "common.h"
#include "mempool.h"
#include "mutexlock.h"

typedef struct queue_s queue_t;
struct queue_s
{
    queue_t* prev;
    queue_t* next;
    VOID* data;
};

class Cqueue
{
public:
    Cqueue(INT32 itemsize);
    ~Cqueue();
    bool empty();
    bool isempty();
    bool pushHead(VOID* data);
    bool pushEnd(VOID* data);
    VOID* popHead();
    VOID* popEnd();
    VOID* pop();
    VOID  popReset();
    bool remove(VOID* data);
    VOID* qalloc(size_t size);
    INT32 getsize();
private:
    bool isFreeMqueueEmpty();
    queue_t *mqueue;
    queue_t *freemqueue;
    queue_t* poptemp;
    Mempool mpool;
    INT32 itemsize;
    MutexLock mlock;
    INT32 size;
};
#endif