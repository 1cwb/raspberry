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
    TcpServer server;
    server.setAddr(INADDR_ANY);
    server.setFamily(AF_INET);
    server.setPort(40960);
    if(!server.start())
    {
        LOG_ERROR("start Tcp Server failuer!");
        return -1;
    }
    struct sockaddr_in servaddr;
    socklen_t servlen = sizeof(servaddr);
    if(Socket::Cgetsockname(server.getSockfd(), (struct sockaddr*)&servaddr, &servlen) == 0)
    {
        printf("ip is %d\n",servaddr.sin_addr.s_addr);
        CHAR IP[64] = {'\0'};
        const char *p = NULL;
        if(p = inet_ntop(servaddr.sin_family,&(servaddr.sin_addr),IP,64))
        printf("family is %d, ip is %d, port is %d p is %s\n",servaddr.sin_family,servaddr.sin_addr.s_addr,Socket::Cntohs(servaddr.sin_port) ,p);
        else
        {
            printf("error --- %s\n",Cstrerror(errno));
        }
    }
    INT32 connfd;
    CHAR sendBuff[1024] = {0};
    INT32 N = 0;
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    
    while(true)
    {
        connfd = server.accept((struct sockaddr*)&cliaddr, &clilen);
        if(connfd < 0)
        {
            continue;
        }
     servlen = sizeof(servaddr);
    memset(&servaddr,0,sizeof(struct sockaddr_in));
        if(Socket::Cgetsockname(server.getSockfd(), (struct sockaddr*)&servaddr, &servlen) == 0)
    {
        printf("ip is %d\n",servaddr.sin_addr.s_addr);
        CHAR IP[64] = {'\0'};
        const char *p = NULL;
        if(p = inet_ntop(servaddr.sin_family,&(servaddr.sin_addr),IP,64))
        printf("family is %d, ip is %d, port is %d p is %s\n",servaddr.sin_family,servaddr.sin_addr.s_addr,Socket::Cntohs(servaddr.sin_port) ,p);
        else
        {
            printf("error --- %s\n",Cstrerror(errno));
        }
    }
    if(Socket::Cgetpeername(connfd, (struct sockaddr*)&servaddr, &servlen) == 0)
    {
        printf("ip is %d\n",servaddr.sin_addr.s_addr);
        CHAR IP[64] = {'\0'};
        const char *p = NULL;
        if(p = inet_ntop(servaddr.sin_family,&(servaddr.sin_addr),IP,64))
        printf("family is %d, ip is %d, port is %d p is %s\n",servaddr.sin_family,servaddr.sin_addr.s_addr,Socket::Cntohs(servaddr.sin_port) ,p);
        else
        {
            printf("error --- %s\n",Cstrerror(errno));
        }
    }
        if(read(connfd,sendBuff,1024) > 0)
        {
            write(connfd,sendBuff, 1024);
            //close(connfd);
            
        }
    }
    
    return 0;
}
