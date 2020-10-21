#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "errno.h"
#include <arpa/inet.h>
#include "cselect.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP server\n");
    NetServer server(AF_INET6);
    Cselect mslect;
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
    INT32 connfd = -1;
    //INT32 selectNum = server.getsockFD() + 1;
    INT32 maxfd = server.getsockFD();
    mslect.fdSet(server.getsockFD(),READ_FD_EM);
    while(true)
    {
        if(mslect.selectfd(maxfd + 1, NULL) < 1)
        {
            LOG_ERROR("select return failed"); 
            continue;
        }
        for(INT32 fd = 0; fd < maxfd + 1; fd ++)
        {
            if(mslect.fdIsSet(fd,READ_FD_EM))
            {
                if(fd == server.getsockFD())
                {
                    struct sockaddr_storage clintaddr;
                    socklen_t clen = sizeof(struct sockaddr_storage);
                    memset(&clintaddr, 0, clen);
                    connfd = server.Caccept((struct sockaddr*)&clintaddr, &clen);
                    mslect.fdSet(server.getsockFD(), READ_FD_EM);
                    if(connfd < 0)
                    {
                        continue;
                    }
                    mslect.fdSet(connfd,READ_FD_EM);
                    maxfd = connfd > maxfd ? connfd : maxfd;
                    CHAR straddr[32] = {0};
                    LOG_INFO("clinet is %s",NetTool::CinetNtop(clintaddr.ss_family, &(((struct sockaddr_in6*)(&clintaddr))->sin6_addr),straddr, 32 ));
                    INT32 n = write(connfd, "欢迎使用联发科人工智能系统>>>", sizeof("欢迎使用联发科人工智能系统>>>"));
                    if(n < 0)
                    {
                        LOG_ERROR("write date to client failer!");
                    }
                }
                else
                {
                    mslect.fdSet(fd,READ_FD_EM);
                    CHAR recvbuff[512] = {0};
                    INT32 ncount = 0;
                    if(ncount = read(fd, recvbuff, 512) > 0)
                    {
                        if(strcmp(recvbuff,"你好") >= 0)
                        {
                            write(fd, "你好,这里是联发科技人工智能中心,很高兴为您服务!",sizeof("你好,这里是联发科技人工智能中心,很高兴为您服务!"));
                        }
                    }
                    else if(ncount == 0)
                    {
                        close(fd);
                    }
                    else 
                    {
                        LOG_ERROR("something was wrong~~");
                    }
                }
                
            }
        }
        
        


        //NetTool::Cshutdown(connfd,SHUT_RD);
        //close(connfd);
        //LOG_INFO("connect over!");
    }
    return 0;
}
