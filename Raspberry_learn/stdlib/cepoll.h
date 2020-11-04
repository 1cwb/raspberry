#ifndef CEPOLL_H
#define CEPOLL_H

#include "common.h"
#include <sys/epoll.h>
//typedef union epoll_data cepoll_data_t;
//typedef struct epoll_event cepoll_event;
/*
op: EPOLL_CTL_ADD/EPOLL_CTL_MOD/EPOLL_CTL_DEL
typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  uint32_t events;	// Epoll events 
  epoll_data_t data; //User data variable 
} __EPOLL_PACKED;
events:EPOLLIN  //fd can read
       EPOLLOUT //fd can write
       EPOLLPRI //fd has busy data can read
       EPOLLERR //fd error
       EPOLLHUP //
       EPOLLET  //edge triggered
       //EPOLLLT  //level triggred
       EPOLLONESHOT //listen event one time
*/

class Cepoll
{
public:
    Cepoll(INT32 size);
    ~Cepoll();
    bool isCepollCanbeUse();
    INT32 CepollCtl(INT32 op, INT32 fd, struct epoll_event* event);
    INT32 CepollWait(struct epoll_event* events, INT32 maxevents, INT32 timeout);
    INT32 CgetEpFd();
    INT32 Cclose();
private:
    INT32 epfd;
    bool canbeused;
};
#endif