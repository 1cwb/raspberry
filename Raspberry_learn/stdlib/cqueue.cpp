#include "cqueue.h"
#include <string.h>
Cqueue::Cqueue(INT32 itemsize):mqueue(NULL), freemqueue(NULL), poptemp(NULL), mpool(4096), itemsize(itemsize)
{
    /*mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
    mqueue->data = NULL;
    mqueue->next = mqueue;
    mqueue->prev = mqueue;*/
}
Cqueue::~Cqueue()
{
    mqueue = NULL;
    freemqueue = NULL;
}
bool Cqueue::empty()
{
    mqueue = NULL;
    freemqueue = NULL;
    mpool.resetPool();
}
bool Cqueue::isempty()
{
    return mqueue == NULL;
}
bool Cqueue::pushHead(VOID* data)
{
    VOID* mdata = NULL;
    if(mqueue == NULL)
    {
        if(freemqueue == NULL)
        {
            mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(mqueue == NULL)
            {
                return false;
            }
            mdata = mpool.palloc(itemsize);
            if(mdata == NULL )
            {
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
                return false;
            }
            mdata = mpool.palloc(itemsize);
            if(mdata == NULL )
            {
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
    if(mqueue == NULL)
    {
        if(freemqueue == NULL)
        {
            mqueue = (queue_t*)mpool.palloc(sizeof(queue_t));
            if(mqueue == NULL)
            {
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
    return true;
}
VOID* Cqueue::popHead()
{
    if(isempty())
    {
        return NULL;
    }
    return mqueue->data;
}
VOID* Cqueue::popEnd()
{
    if(isempty())
    {
        return NULL;
    }
    return mqueue->prev->data;
}

bool Cqueue::remove(VOID* data)
{
    if(mqueue == NULL)
    {
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
        return false;
    }
    return true;
}
VOID* Cqueue::pop()
{
    queue_t *temp = poptemp;
    if(isempty() || temp == NULL)
    {
        mpool.debug();
        return NULL;
    }
    poptemp = poptemp->next;
    if(poptemp == mqueue)
    {
        poptemp = NULL;
    }  
    return temp->data;
}
VOID  Cqueue::popReset()
{
    poptemp = mqueue;
}

    // bool isFreeMqueueEmpty();
    // queue_t mqueue;
    // queue_t freemqueue;
    // Mempool mpool; 