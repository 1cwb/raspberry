#ifndef MSG_H
#define MSG_H
#include "common.h"
#include "mutexlock.h"
#include "condition.h"


#define MAX_QUEUE_SIZE_IN_BYTES (1024)
#define MQ_SIZE_MAX 512
#define MQ_LENGTH_MAX 30
#define MQ_NAME "msg queue example"

typedef enum _queue_type
{
    QUEUE_BLOCK,
    QUEUE_NO_BLOCK
}queue_type;
/*
typedef enum _queue_status
{
    QUEUE_IS_NORMAL,
    QUEUE_NO_EXIT,
    QUEUE_IS_FULL,
    QUEUE_IS_EMPTY
}queue_status;
*/
typedef enum _cntl_queue_ret
{
    CNTL_QUEUE_SUCESSFUL,
    CNTL_QUEUE_FAIL,
    CNTL_QUEUE_TIMEOUT,
    CNTL_QUEUE_PARAM_ERROR,
    CNTL_QUEUE_CANCEL,
    CNTL_QUEUE_INVALIED
}cntl_queue_ret;

typedef enum _queue_flag
{
    IPC_BLOCK,
    IPC_NOWAIT,
    IPC_WAITTIMES
}queue_flag;

typedef struct _simple_queue_buf
{
    INT32 msg_type;
    CHAR msg_buf[64];
}queue_buf;
class Msg
{
public:
    Msg(const CHAR* queue_name, INT32 length, INT32 type);
    ~Msg();
    bool isFullMsgQueue();
    bool isEmptyMsgQueue();
    cntl_queue_ret push(VOID* data, ULONG nms, queue_flag flg);
    cntl_queue_ret pop(VOID** data, ULONG nms, queue_flag flg);
    VOID setMsgCancel();
    const CHAR* getQueueName();
    bool formatMsg(queue_buf* buff, const CHAR* data, INT32 len, MSG_EVENT envt);
private:
    INT32 front;
    INT32 rear;
    INT32 length;
    INT32 queue_type;
    MutexLock mutex;
    Condition cond;
    CHAR queue_name[32];
    U_PTR* data;
    bool isMsgCancel;
};


#endif
