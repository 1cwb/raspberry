#ifndef NETPOLL_H
#define NETPOLL_H
#include "cepoll.h"
#inlcude "common.h"

#define MAX_EPOLL_EVENTS 2000

class Netpoll
{
public:
    Netpoll(/* args */);
    ~Netpoll();
    VOID addChannel(Channel* ch);
    VOID removeChannel(Channel* ch);
    VOID updateChannel(Channel* ch);
    VOID loop_once(INT32 waitMs);
private:
    /* data */
    struct epoll_event activeEvs[MAX_EPOLL_EVENTS];
    Cepoll mepoll;
};



#endif