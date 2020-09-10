#include "cepoll.h"
#include <unistd.h>
Cepoll::Cepoll(INT32 size): epfd(-1),canbeused(false)
{
    if((epfd = epoll_create(8)) >= 0)
    {
        canbeused = true;
    }
}
Cepoll::~Cepoll()
{
    if(epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
}
INT32 Cepoll::CepollCtl(INT32 op, INT32 fd, struct epoll_event* event)
{
    return epoll_ctl(epfd, op, fd,  event);
}
INT32 Cepoll::CepollWait(struct epoll_event* event, INT32 maxevents, INT32 timeout)
{
    return epoll_wait(epfd, event, maxevents, timeout);
}

//INT32 epfd;