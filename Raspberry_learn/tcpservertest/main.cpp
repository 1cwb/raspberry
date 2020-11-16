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
#include "netpoll.h"

using namespace std;

#define RECVBUFF_MAX_LEN (1024 * 512) //16k
#define MAX_FD_CLIENT 1024

Msg lconnect("connect", 1024);

NetServer server(AF_INET6);
//Cepoll mepoll(1);
Netpoll mpoll;

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
        Channel ch(newfd, &mpoll);
        ch.CenableRead(true);
    }
    return NULL;
}
static VOID* workerThreadFunc(VOID* data)
{
    Channel* ch = NULL;
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
        //INT32 *fd = NULL;
        lconnect.pop((VOID**)&ch, 0, IPC_BLOCK);
        if(ch == NULL) {printf("get NULL data\n\n");continue;}
        //free(fd);
        memset(recvdata, 0, sizeof(CHAR) * RECVBUFF_MAX_LEN);
        while(true)
        {
            memset(buff, 0, sizeof(buff));
            INT32 nRecv = ch->Crecv( buff, 256, 0);
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
                    ch->CcloseConnect();
                    berror = true;
                    break;
                }
            }
            else if(nRecv == 0)
            {
                LOG_INFO("client close fd");
                ch->CcloseConnect();
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
            INT32 nSend = ch->Csent(recvdata, strlen(recvdata) + 1, 0);
            if(nSend < 0)
            {
                if(errno == EWOULDBLOCK)
                {
                    retrycnt ++;
                    if(retrycnt > 3)
                    {
                        LOG_ERROR("send error, client disconnect");
                        ch->CcloseConnect();
                        break;
                    }
                    sleep(5);
                    continue;
                }
                else
                {
                    LOG_ERROR("send error, client disconnect");
                    ch->CcloseConnect();
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
    /*if(!mepoll.isCepollCanbeUse())
    {
        LOG_ERROR("init lize epoll fail");
        return -1;
    }*/
    Channel listenCh(server.getsockFD(), &mpoll);
    NetTool::Csetfdblock(server.getsockFD(), false);
    listenCh.CenableRead(true);

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
        INT32 active = mpoll.NetepollWait(5000);
        if(active == 0)
        {
            LOG_INFO("Current conenct clients num Is %d",mpoll.getConnectClientNum());
            continue;
        }
        else if(active < 0)
        {
            LOG_ERROR("epoll wait error!");
            continue;
        }
        struct epoll_event *ev = mpoll.getEpollEvents();
        for(INT32 temp = 0; temp < active; temp++)
        {
            if(((Channel*)(ev[temp].data.ptr))->CgetFD() == server.getsockFD())
            {
                macceptCond.notify();
            }
            else
            {
                //mClientMutex.lock();
                //lconnect.push_back(ev[temp].data.fd);
                //mClientMutex.unlock();
                //mCond.notify();
                //INT32* nfd = (INT32*)malloc(sizeof(INT32));
                //if(nfd == NULL)
                //{
               //     LOG_ERROR("MALLOC FAIL !!!!!!!!!!\n");
                //}
                //*nfd = ev[temp].data.fd;
                //lconnect.push(nfd, 500, IPC_WAITTIMES);
                if(ev[temp].events & EPOLLIN)
                {
                    LOG_DEBUG("ev is %p",&ev[temp]);
                    lconnect.push(ev[temp].data.ptr, 500, IPC_WAITTIMES);
                } 
            }
        }
    }
    server.Cshutdown();
    server.CcloseSockfd();
    lconnect.setMsgCancel();
    return 0;
}
