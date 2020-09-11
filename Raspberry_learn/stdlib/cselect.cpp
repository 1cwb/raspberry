#include "cselect.h"
#include <string.h>
#include "clog.h"
Cselect::Cselect()
{
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
}
Cselect::~Cselect()
{

}
bool Cselect::fdIsSet(INT32 fd, FD_TYPE type)
{
    if(type == READ_FD_EM)
    {
       return FD_ISSET(fd, &readfds);
    }
    else if(type == WRITE_FD_EM)
    {
        return FD_ISSET(fd, &writefds);
    }
    else if(type == EXCEP_FD_EM)
    {
        return FD_ISSET(fd, &exceptfds);
    }
    return false;
}
VOID Cselect::fdClear(INT32 fd, FD_TYPE type)
{
    if(type == READ_FD_EM)
    {
       FD_CLR(fd, &readfds);
    }
    else if(type == WRITE_FD_EM)
    {
        FD_CLR(fd, &writefds);
    }
    else if(type == EXCEP_FD_EM)
    {
        FD_CLR(fd, &exceptfds);
    } 
}
VOID Cselect::fdSet(INT32 fd, FD_TYPE type)
{
    if(type == READ_FD_EM)
    {
       FD_SET(fd, &readfds);
    }
    else if(type == WRITE_FD_EM)
    {
        FD_SET(fd, &writefds);
    }
    else if(type == EXCEP_FD_EM)
    {
        FD_SET(fd, &exceptfds);
    } 
}
VOID Cselect::fdZero(FD_TYPE type)
{
    if(type == READ_FD_EM)
    {
       FD_ZERO(&readfds);
    }
    else if(type == WRITE_FD_EM)
    {
        FD_ZERO(&writefds);
    }
    else if(type == EXCEP_FD_EM)
    {
        FD_ZERO(&exceptfds);
    } 
}
VOID Cselect::fdZeroAll()
{
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
}
INT32 Cselect::selectfd(INT32 maxfdpl, struct timeval tvptr)
{
    return select(maxfdpl, &readfds, &writefds, &exceptfds, &tvptr);
}
//private:
//fd_set readfds;
//fd_set writefds;
//fd_set exceptfds;
//struct timeval tvptr;