#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "errno.h"
#include "cepoll.h"
#include "thread.h"
#include "condition.h"
#include <iostream>
#include <list>
#include "msg.h"
#include "mempool.h"
#include "cqueue.h"
using namespace std;

#define RECVBUFF_MAX_LEN (1024 * 512) //16k
#define MAX_FD_CLIENT 1024

Msg lconnect("connect", 1024);

NetServer server(AF_INET6);
Cepoll mepoll(1);
MutexLock macceptMutex;
Condition macceptCond(macceptMutex);
//MutexLock mClientMutex;
//Condition mCond(mClientMutex);
//list<INT32> lconnect;
bool bstop = false;

static VOID* acceptThreadFunc(VOID* data)
{
    INT32 newfd = -1;
    while(!bstop)
    {
        macceptMutex.lock();
        macceptCond.wait();
        newfd = server.Caccept(NULL, NULL);
        macceptMutex.unlock();
        if(newfd < 0)
        {
            continue;
        }
        if(!NetTool::Csetfdblock(newfd, false))
        {
            continue;
        }
        struct epoll_event e;
        e.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        e.data.fd = newfd;
        mepoll.CepollCtl(EPOLL_CTL_ADD, newfd, &e);
    }
    return NULL;
}
static VOID* workerThreadFunc(VOID* data)
{
    INT32 clientfd = -1;
    char buff[256];
    bool berror = false;
    INT32 retrycnt = 0;
    CHAR *recvdata = (CHAR*)malloc(sizeof(CHAR) * RECVBUFF_MAX_LEN);
    if(recvdata == NULL)
    {
        LOG_ERROR("malloc recvdata fail!");
        bstop = true;
        return NULL;
    }
    while(!bstop)
    {
        /*mClientMutex.lock();
        while(lconnect.empty())
        {
            mCond.wait();
        }
        clientfd = lconnect.front();
        lconnect.pop_front();
        mClientMutex.unlock();*/
        INT32 *fd = NULL;
        lconnect.pop((VOID**)&fd, 0, IPC_BLOCK);
        if(fd == NULL) {printf("get NULL data\n\n");continue;}
        clientfd = *fd;
        //printf("pop get fd is %d\n",clientfd);
        free(fd);
        memset(recvdata, 0, sizeof(CHAR) * RECVBUFF_MAX_LEN);
        while(true)
        {
            memset(buff, 0, sizeof(buff));
            INT32 nRecv = recv(clientfd, buff, 256, 0);
            if(nRecv < 0)
            {
                if(errno == EWOULDBLOCK)
                {
                    LOG_DEBUG("there is no data");
                    break;
                }
                else
                {
                    LOG_ERROR("recv error, client disconnect");
                    mepoll.CepollCtl(EPOLL_CTL_DEL, clientfd, NULL);
                    close(clientfd);
                    berror = true;
                    break;
                }
            }
            else if(nRecv == 0)
            {
                LOG_INFO("client close fd");
                mepoll.CepollCtl(EPOLL_CTL_DEL, clientfd, NULL);
                close(clientfd);
                berror = true;
                break;
            }
            strcat(recvdata, buff);
        }
        if(berror)
        {
            continue;
        }
        LOG_INFO("RECV: %s",recvdata);
        retrycnt = 0;
        while(true)
        {
            strcat(recvdata, "--server reply!");
            INT32 nSend = send(clientfd, recvdata, strlen(recvdata) + 1, 0);
            if(nSend < 0)
            {
                if(errno == EWOULDBLOCK)
                {
                    retrycnt ++;
                    if(retrycnt > 3)
                    {
                        LOG_ERROR("send error, client disconnect");
                        mepoll.CepollCtl(EPOLL_CTL_DEL, clientfd, NULL);
                        close(clientfd);
                        break;
                    }
                    sleep(5);
                    continue;
                }
                else
                {
                    LOG_ERROR("send error, client disconnect");
                    mepoll.CepollCtl(EPOLL_CTL_DEL, clientfd, NULL);
                    close(clientfd);
                    break;
                }
            }
            break;
        }
    }
    if(recvdata != NULL)
    {
        free(recvdata);
    }
    return NULL;
}
int main(INT32 argc, CHAR** argv)
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP server\n");
    LOG_DEBUG("server.getsockFD() %d",server.getsockFD());
    NetTool::CsetReusePort(server.getsockFD(), true);
    NetTool::CsetReuseAddr(server.getsockFD(),true);
    if(!server.initAddr(40960,NULL))
    {
        LOG_ERROR("Init addr fail!");
        return -1;
    }
    if(!server.start())
    {
        LOG_ERROR("start to run server fail!");
        return -1;
    }
    if(!mepoll.isCepollCanbeUse())
    {
        LOG_ERROR("init lize epoll fail");
        return -1;
    }
    struct epoll_event e;
    e.events = EPOLLIN | EPOLLRDHUP;
    e.data.fd = server.getsockFD();
    if(mepoll.CepollCtl(EPOLL_CTL_ADD, server.getsockFD(), &e) < 0)
    {
        LOG_ERROR("add listenfd to epoll fail!,error %s",Cstrerror(errno));
        return -1;
    }
    
    Thread acceptThread;
    acceptThread.setThreadFunc(acceptThreadFunc,NULL);
    acceptThread.start();
    Thread workerThread[64];
    for(INT32 i = 0; i < 64; i++)
    {
        workerThread[i].setThreadFunc(workerThreadFunc, NULL);
        workerThread[i].start();
    }
    while(!bstop)
    {
        struct epoll_event ev[1024];
        INT32 n = mepoll.CepollWait(ev,1024, -1);
        if(n == 0)
        {
            LOG_INFO("what happend?");
            continue;
        }
        else if(n < 0)
        {
            LOG_ERROR("epoll wait error!");
            continue;
        }
        INT32 active = MIN(n, 1024);
        for(INT32 temp = 0; temp < active; temp++)
        {
            if(ev[temp].data.fd == server.getsockFD())
            {
                macceptCond.notify();
            }
            else
            {
                //mClientMutex.lock();
                //lconnect.push_back(ev[temp].data.fd);
                //mClientMutex.unlock();
                //mCond.notify();
                INT32* nfd = (INT32*)malloc(sizeof(INT32));
                if(nfd == NULL)
                {
                    LOG_ERROR("MALLOC FAIL !!!!!!!!!!\n");
                }
                *nfd = ev[temp].data.fd;
                lconnect.push(nfd, 500, IPC_WAITTIMES);
            }
        }
    }
    mepoll.Cclose();
    server.Cshutdown();
    server.CcloseSockfd();
    lconnect.setMsgCancel();
    return 0;
}
