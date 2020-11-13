#include "netpoll.h"
#include <string.h>

Netpoll::Netpoll():mepoll(1)
{
    if(!mepoll.isCepollCanbeUse())
    {
        printf("epoll init fail!\n");
    }
}
Netpoll::~Netpoll()
{
    mepoll.Cclose();
}
VOID Netpoll::addChannel(Channel* ch)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = ch->CgetEvents();
    ev.data.ptr = ch;
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
VOID Netpoll::loop_once(INT32 waitMs)
{
    INT32 lastActive = mepoll.CepollWait(activeEvs, MAX_EPOLL_EVENTS, waitMs);
    while(--lastActive >= 0)
    {
        INT32 i = lastActive;
        Channel* ch = (Channel*)activeEvs[i].data.ptr;
        INT32 events = activeEvs[i].events;
        if(ch != NULL)
        {
            if(events & (EPOLLIN | EPOLLERR))
            {
                //ch->handleRead();
            }
            else if(events & EPOLLOUT)
            {
                //ch->handleWrite();
            }
            else
            {
                /* code */
            }
            
        }
    }
}

    //struct epoll_event activeEvs[MAX_EPOLL_EVENTS];
    //Cepoll mepoll;