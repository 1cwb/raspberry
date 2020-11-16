#include "netpoll.h"
#include <string.h>

Netpoll::Netpoll():mepoll(1), cmqueue(sizeof(Channel))
{
    if(!mepoll.isCepollCanbeUse())
    {
        printf("epoll init fail!\n");
    }
}
Netpoll::~Netpoll()
{
    cmqueue.empty();
    mepoll.Cclose();
}
VOID Netpoll::addChannel(Channel* ch)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));

    cmqueue.pushEnd(ch);
    ev.events = ch->CgetEvents();
    ev.data.ptr = cmqueue.popEnd();
    if(mepoll.CepollCtl(EPOLL_CTL_ADD, ch->CgetFD(), &ev) != 0)
    {
        printf("epoll ADD fail!");
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
}
VOID Netpoll::removeChannel(Channel* ch)
{
    if(mepoll.CepollCtl(EPOLL_CTL_DEL, ch->CgetFD(), NULL) != 0)
    {
        printf("epoll DELETE fail!");
    }
	cmqueue.remove(ch);
}
VOID Netpoll::updateChannel(Channel* ch)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->CgetEvents();
    ev.data.ptr = ch;
    if(mepoll.CepollCtl(EPOLL_CTL_MOD, ch->CgetFD(), &ev) != 0)
    {
        printf("epoll MOD fail!");
    }
}
INT32 Netpoll::NetepollWait(INT32 waitMs)
{
    INT32 active = mepoll.CepollWait(activeEvs, MAX_EPOLL_EVENTS, waitMs);
    return (active < MAX_EPOLL_EVENTS ? active : MAX_EPOLL_EVENTS);
}

struct epoll_event* Netpoll::getEpollEvents()
{
    return activeEvs;
}
INT32 Netpoll::getConnectClientNum()
{
    return cmqueue.getsize();
}
    //struct epoll_event activeEvs[MAX_EPOLL_EVENTS];
    //Cepoll mepoll;