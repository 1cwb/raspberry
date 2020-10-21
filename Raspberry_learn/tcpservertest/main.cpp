#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "errno.h"
#include "cselect.h"
#define MAX_FD_CLIENT 1024
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP server\n");
    NetServer server(AF_INET6);
    Cselect mslect, mnewselect;
    INT32 clientfd[MAX_FD_CLIENT];
    INT32 nclientcount = 0;
    INT32 nready = 0;
    INT32 connfd = 0;
    INT32 maxfd  = 0;
    NetTool::CsetReuseAddr(server.getsockFD(), true);
    NetTool::CsetReusePort(server.getsockFD(), true);
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
    maxfd = server.getsockFD();
    mnewselect.fdSet(server.getsockFD(),READ_FD_EM);
    memset(clientfd, -1, sizeof(INT32) * MAX_FD_CLIENT);
    while(true)
    {
        mslect.fdSet(&mnewselect, READ_FD_EM);
        nready = mslect.selectfd(maxfd + 1, NULL);
        if(mslect.fdIsSet(server.getsockFD(),READ_FD_EM))
        {
            struct sockaddr_storage clintaddr;
            socklen_t clen = sizeof(struct sockaddr_storage);
            memset(&clintaddr, 0, clen);
            connfd = server.Caccept((struct sockaddr*)&clintaddr, &clen);
            if(connfd < 0)
            {
                continue;
            }
            for(nclientcount = 0; nclientcount < MAX_FD_CLIENT; nclientcount ++)
            {
                LOG_INFO("nclientcount is %d",nclientcount);
                if(clientfd[nclientcount] < 0)
                {
                    clientfd[nclientcount] = connfd;
                    break;
                }
            }
            if(nclientcount == MAX_FD_CLIENT)
            {
                LOG_WARNING("Too many clients!!");
            }
            mnewselect.fdSet(connfd, READ_FD_EM);
            maxfd = connfd > maxfd ? connfd : maxfd;
            CHAR straddr[32] = {0};
            LOG_INFO("clinet is %s",NetTool::CinetNtop(clintaddr.ss_family, &(((struct sockaddr_in6*)(&clintaddr))->sin6_addr),straddr, 32 ));
            INT32 n = write(connfd, "欢迎使用联发科人工智能系统>>>", sizeof("欢迎使用联发科人工智能系统>>>"));
            if(n < 0)
            {
                LOG_ERROR("write date to client failer!");
            }
            if(--nready <= 0)
            {
                continue;
            }
        }

        for(nclientcount = 0; nclientcount < maxfd + 1; nclientcount ++)
        {
            if(clientfd[nclientcount] < 0)
            {
                continue;
            }
            if(mslect.fdIsSet(clientfd[nclientcount], READ_FD_EM))
            {
                CHAR recvbuff[256] = {0};
                CHAR sendbuff[256] = {0};
                INT32 n;
                n = read(clientfd[nclientcount], recvbuff, 256);
                if(n == 0)
                {
                    close(clientfd[nclientcount]);
                    mnewselect.fdClear(clientfd[nclientcount], READ_FD_EM);
                    clientfd[nclientcount] = -1;
                }
                else
                {
                    if(strcmp(recvbuff,"你好") == 0)
                    {
                        n = write(clientfd[nclientcount], "MTK_AI助手: 您好,很高兴为您服务\n", sizeof("MTK_AI助手: 您好,很高兴为您服务\n"));
                        if(n <= 0)
                        {
                            LOG_ERROR("write date to client failer!");
                            close(clientfd[nclientcount]);
                            mnewselect.fdClear(clientfd[nclientcount], READ_FD_EM);
                            clientfd[nclientcount] = -1;
                        }
                    }
                    if(strcmp(recvbuff,"1024是什么节日") == 0)
                    {
                        n = write(clientfd[nclientcount], "MTK_AI助手: 屌丝节...\n", sizeof("MTK_AI助手: 屌丝节...\n"));
                        if(n <= 0)
                        {
                            LOG_ERROR("write date to client failer!");
                            close(clientfd[nclientcount]);
                            mnewselect.fdClear(clientfd[nclientcount], READ_FD_EM);
                            clientfd[nclientcount] = -1;
                        }
                    }
                    if(strcmp(recvbuff,"你再说一遍试试") == 0)
                    {
                        n = write(clientfd[nclientcount], "MTK_AI助手: emmm 程序员节\n", sizeof("MTK_AI助手: emmm 程序员节\n"));
                        if(n <= 0)
                        {
                            LOG_ERROR("write date to client failer!");
                            close(clientfd[nclientcount]);
                            mnewselect.fdClear(clientfd[nclientcount], READ_FD_EM);
                            clientfd[nclientcount] = -1;
                        }
                    }
                }
                if(--nready <= 0)
                {
                    break;
                }
            }
        }
    }
    return 0;
}
