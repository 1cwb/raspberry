#include "cepoll.h"
#include <unistd.h>
#include "clog.h"
Cepoll::Cepoll(INT32 size): epfd(-1),canbeused(false)
{
    if((epfd = epoll_create(size)) >= 0)
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
    canbeused = false;
}
INT32 Cepoll::CepollCtl(INT32 op, INT32 fd, struct epoll_event* event)
{
    return epoll_ctl(epfd, op, fd, event);
}
INT32 Cepoll::CepollWait(struct epoll_event* events, INT32 maxevents, INT32 timeout)
{
    return epoll_wait(epfd, events, maxevents, timeout);
}
bool Cepoll::isCepollCanbeUse()
{
    return canbeused;
}
INT32 Cepoll::CgetEpFd()
{
    return epfd;
}
INT32 Cepoll::Cclose()
{
    return close(epfd);
}
//INT32 epfd;