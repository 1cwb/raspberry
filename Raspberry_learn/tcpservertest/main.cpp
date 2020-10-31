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

#define MAX_FD_CLIENT 1024

static VOID* epollThreadFunc(VOID* data)
{

    return NULL;
}

int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP server\n");
    NetServer server(AF_INET6);
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
    Thread epollThread;
    Cepoll mepoll(0);
    epollThread.setThreadFunc(epollThreadFunc,&mepoll);

    
    while(true)
    {
        INT32 connectfd = server.Caccept(NULL, NULL);
        if(connectfd < 0)
        {
            LOG_ERROR("connetc fail, continue");
            continue;
        }

        Connect* mconnect = new Connect(connectfd);
        epoll_event event = {
            
        };
        mepoll.CepollCtl(, connectfd, )
        LOG_INFO("CLIENT IP is %s, port is %d",mconnect->CgetAddrStr(), mconnect->CgetPort());
    }
    return 0;
}
