#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "errno.h"
#include <arpa/inet.h>
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP server\n");
    TcpServer server(AF_INET6);
    if(!server.initAddr(40960,NULL))
    {
        LOG_ERROR("Init Server Failer!");
        return -1;
    }
    if(!server.start())
    {
        LOG_ERROR("start Tcp Server failuer!");
        return -1;
    }
    INT32 connfd = -1;
    while(true)
    {
        connfd = server.Caccept(NULL, NULL);
        if(connfd < 0)
        {
            continue;
        }
        INT32 n = write(connfd, "welcome to vistit this sex web", 31);
        if(n < 0)
        {
            LOG_ERROR("write date to client failer!");
        }
        NetTool::Cshutdown(connfd,SHUT_RD);
        close(connfd);
        LOG_INFO("connect over!");
    }
    return 0;
}
