#include "cqueue.h"
#include <string.h>
Cqueue::Cqueue(INT32 itemsize):mqueue(NULL), freemqueue(NULL), poptemp(NULL), mpool(4096), itemsize(itemsize), size(0)
{
    /*mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
    mqueue->data = NULL;
    mqueue->next = mqueue;
    mqueue->prev = mqueue;*/
}
Cqueue::~Cqueue()
{
    mlock.lock();
    mqueue = NULL;
    freemqueue = NULL;
    mlock.unlock();
}
bool Cqueue::empty()
{
    mlock.lock();
    mqueue = NULL;
    freemqueue = NULL;
    mlock.unlock();
    size = 0;
    mpool.resetPool();
    return true;
}
bool Cqueue::isempty()
{
    return mqueue == NULL;
}
bool Cqueue::pushHead(VOID* data)
{
    VOID* mdata = NULL;
    mlock.lock();
    if(mqueue == NULL)
    {
        if(freemqueue == NULL)
        {
            mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(mqueue == NULL)
            {
                mlock.unlock();
                return false;
            }
            mdata = mpool.palloc(itemsize);
            if(mdata == NULL )
            {
                mlock.unlock();
                return false;
            }
        }
        else
        {
            mdata = freemqueue->data;
            mqueue = freemqueue;
            freemqueue = freemqueue->next;
        }
        memcpy(mdata, data, itemsize);
        mqueue->data = mdata;
        mqueue->next = mqueue;
        mqueue->prev = mqueue;
    }
    else
    {
        queue_t* temp = NULL;
        if(freemqueue == NULL)
        {
            temp = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(temp == NULL)
            {
                mlock.unlock();
                return false;
            }
            mdata = mpool.palloc(itemsize);
            if(mdata == NULL )
            {
                mlock.unlock();
                return false;
            }
        }
        else
        {
            mdata = freemqueue->data;
            temp = freemqueue;
            freemqueue = freemqueue->next;     
        }
        memcpy(mdata, data, itemsize);
        temp->data = mdata;
        temp->next = mqueue->next;
        temp->next->prev = temp;
        temp->prev = mqueue;
        mqueue->next = temp;
    }
    size ++;
    mlock.unlock();
    return true;
}
bool Cqueue::pushEnd(VOID* data)
{
    VOID* mdata = mpool.palloc(itemsize);
    if(mdata == NULL )
    {
        return false;
    }
    memcpy(mdata, data, itemsize);
    mlock.lock();
    if(mqueue == NULL)
    {
        if(freemqueue == NULL)
        {
            mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(mqueue == NULL)
            {
                mlock.unlock();
                return false;
            }
        }
        else
        {
            mqueue = freemqueue;
            freemqueue = freemqueue->next;
        }
        mqueue->data = mdata;
        mqueue->next = mqueue;
        mqueue->prev = mqueue;
    }
    else
    {
        queue_t* temp = NULL;
        if(freemqueue == NULL)
        {
            temp = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(temp == NULL)
            {
                mlock.unlock();
                return false;
            }
        }
        else
        {
            temp = freemqueue;
            freemqueue = freemqueue->next;     
        }
        temp->data = mdata;
        temp->next = mqueue;
        temp->prev = mqueue->prev;
        temp->prev->next = temp;
        mqueue->prev = temp;
    }
    size ++;
    mlock.unlock();
    return true;
}
VOID* Cqueue::popHead()
{
    mlock.lock();
    if(isempty())
    {
        mlock.unlock();
        return NULL;
    }
    mlock.unlock();
    return mqueue->data;
}
VOID* Cqueue::popEnd()
{
    mlock.lock();
    if(isempty())
    {
        mlock.unlock();
        return NULL;
    }
    mlock.unlock();
    return mqueue->prev->data;
}

bool Cqueue::remove(VOID* data)
{
    mlock.lock();
    if(isempty())
    {
        mlock.unlock();
        return true;
    }
    queue_t *temp = mqueue;
    do
    {
        if(temp->data == data)
        {
            memset(temp->data, 0, itemsize);
            if(temp == mqueue)
            {
                if(mqueue->next == mqueue)
                {
                    mqueue->next = freemqueue;
                    freemqueue = mqueue;
                    mqueue = NULL;
                    temp = NULL;
                }
                else
                {
                    mqueue = temp->next;
                    temp->next->prev = temp->prev;
                    temp->prev->next = temp->next;
                    temp->next = freemqueue;
                    freemqueue = temp;
                    temp = NULL;
                }
            }
            else
            {
                temp->next->prev = temp->prev;
                temp->prev->next = temp->next;
                temp->next = freemqueue;
                freemqueue = temp;
                temp = NULL;
            }
            break;
        }
        temp = temp->next;
    }while(temp != mqueue);
    if(temp != NULL)
    {
        mlock.unlock();
        return false;
    }
    size --;
    mlock.unlock();
    return true;
}
VOID* Cqueue::pop()
{
    mlock.lock();
    queue_t *temp = poptemp;
    if(isempty() || temp == NULL)
    {
        mpool.debug();
        mlock.unlock();
        return NULL;
    }
    poptemp = poptemp->next;
    if(poptemp == mqueue)
    {
        poptemp = NULL;
    }
    mlock.unlock();
    return temp->data;
}
VOID  Cqueue::popReset()
{
    mlock.lock();
    poptemp = mqueue;
    mlock.unlock();
}
INT32 Cqueue::getsize()
{
    return size;
}
    // bool isFreeMqueueEmpty();
    // queue_t mqueue;
    // queue_t freemqueue;
    // Mempool mpool; 