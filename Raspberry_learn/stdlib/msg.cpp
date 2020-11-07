#include "msg.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "clog.h"

Msg::Msg(const CHAR* queue_name, INT32 length) : 
front(0), rear(0), length(length), cond(mutex), isMsgCancel(false)
{
    assert(queue_name);
    assert(length >= 0);

    if(length > MAX_QUEUE_SIZE_IN_BYTES)
    {
        printf("Warning : length should be less than %d\n",MAX_QUEUE_SIZE_IN_BYTES);
        length = MAX_QUEUE_SIZE_IN_BYTES;
    }
    data = (U_PTR*)malloc(sizeof(U_PTR*) * length);
    assert(data);
    strcpy(this->queue_name,queue_name);
}
Msg::~Msg()
{
    while(front != rear)
    {  
        U_PTR* p = ((U_PTR*)(data + front));
        //free((void*)(*(data + front)));
	    free(p);
        front ++;
        if(front == length)
        {
            front = 0;
        }
    }
    if(data != NULL)
    {
        free(data);
        data = NULL;
    }
}
void Msg::setMsgCancel()
{
    mutex.lock();
    isMsgCancel = true;
    mutex.unlock();
    cond.notifyAll();
}
bool Msg::isFullMsgQueue()
{
    if(((front < rear) && ((rear - front) == (length - 1))) ||
        ((front > rear) && (rear + 1 == front)))
    {
        //printf("MsgQueue is full\n");
        return true;
    }
    return false;
}
bool Msg::isEmptyMsgQueue()
{
    if(front == rear)
    {
        //printf("MsgQueue is empty\n");
        return true;
    }
    return false;
}
cntl_queue_ret Msg::push(VOID* data, ULONG nms, queue_flag flg)
{
    INT32 ret = -2;
    if(data == NULL)
    {
        printf("data is NULL\n");
        return CNTL_QUEUE_PARAM_ERROR;
    }
    mutex.lock();
    if(isFullMsgQueue() && !isMsgCancel)
    {
        if(flg == IPC_BLOCK)
        {
            while(isFullMsgQueue() && !isMsgCancel)
            {
                cond.wait();
            }
        }
        else if(flg == IPC_WAITTIMES)
        {
            while(isFullMsgQueue() && !isMsgCancel && ret != ETIMEDOUT)
            {
                ret = cond.wait(nms);
            }
        }
        else
        {
            //printf("xxMsgQueue is full\n");
            mutex.unlock();
            return CNTL_QUEUE_FAIL;
        }
    }
    if(flg == IPC_WAITTIMES && ret == ETIMEDOUT)
    {
        mutex.unlock();
        return CNTL_QUEUE_TIMEOUT;
    }
    *(this->data + rear) = (U_PTR)((U_PTR*)data);
    rear++;
    if(rear == length)
    {
        rear = 0;
    }
    cond.notify();
    mutex.unlock();
    return isMsgCancel ? CNTL_QUEUE_CANCEL : CNTL_QUEUE_SUCESSFUL;
}
cntl_queue_ret Msg::pop(VOID** data, ULONG nms, queue_flag flg)
{
    INT32 ret = -1;
    mutex.lock();
    if(isEmptyMsgQueue() && !isMsgCancel)
    {
        if(flg == IPC_BLOCK)
        {
            while(isEmptyMsgQueue() && !isMsgCancel)
            {
                cond.wait();
            }
            
        }
        else if(flg == IPC_WAITTIMES)
        {
            while(isEmptyMsgQueue() && !isMsgCancel && ret != ETIMEDOUT)
            {
                ret = cond.wait(nms);//DBG("ret == %d",ret);
            }
        }
        else
        {
            //printf("MsgQueue is empty\n");
            mutex.unlock();
            return CNTL_QUEUE_FAIL;
        }
    }
    if(flg == IPC_WAITTIMES && ret == ETIMEDOUT)
    {
        mutex.unlock();
        return CNTL_QUEUE_TIMEOUT;
    }
    *data = (void*)(*(this->data + front));
    front++;
    if(front == length)
    {
        front = 0;
    }
    cond.notify();
    mutex.unlock();
    return isMsgCancel ? CNTL_QUEUE_CANCEL : CNTL_QUEUE_SUCESSFUL;
}
const CHAR* Msg::getQueueName()
{
    return queue_name;
}

bool Msg::formatMsg(queue_buf* buff, const CHAR* data, INT32 len, MSG_EVENT envt)
{
    if(buff == NULL || data ==NULL || len < 0)
    {
        DBG("Format MSG error!");
        return false;
    }
    memset(buff, '\0', sizeof(queue_buf));
    buff->msg_type = envt;
    memcpy(buff->msg_buf, data, 64);
    return true;
}
