#ifndef NETPOLL_H
#define NETPOLL_H
#include "cepoll.h"
#include "common.h"
#include "msocket.h"
#include "cqueue.h"
#include "thread.h"

#define MAX_EPOLL_EVENTS 2000

class Channel;
class Netpoll
{
public:
    Netpoll(/* args */);
    ~Netpoll();
    VOID addChannel(Channel* ch);
    VOID removeChannel(Channel* ch);
    VOID updateChannel(Channel* ch);
    INT32 NetepollWait(INT32 waitMs);
    struct epoll_event* getEpollEvents();
    INT32 getConnectClientNum();
private:
    /* data */
    struct epoll_event activeEvs[MAX_EPOLL_EVENTS];
    Cepoll mepoll;
	Cqueue cmqueue;
};



#endif