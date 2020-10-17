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
    LOG_DEBUG("Now Start a UDP server\n");
    NetServer server(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    NetTool::CsetReuseAddr(server.getsockFD(), true);
    NetTool::CsetReusePort(server.getsockFD(), true);
    if(!server.initAddr(40960,NULL))
    {
        LOG_ERROR("Init Server Failer!");
        return -1;
    }
    INT32 connfd = -1;
    CHAR strbuff[256];
    CHAR straddr[128] = {0};
    while(true)
    {
        struct sockaddr_storage clintaddr;
        socklen_t clen = sizeof(struct sockaddr_storage);
        memset(&clintaddr, 0, clen);
        memset(strbuff,0,256);
        memset(straddr,0,128);
        NetTool::Crecvfrom(server.getsockFD(),strbuff,256,0,(((struct sockaddr*)(&clintaddr))),&clen);
        LOG_DEBUG("recieve from %s is %s",
        NetTool::CinetNtop(clintaddr.ss_family, &(((struct sockaddr_in6*)(&clintaddr))->sin6_addr),straddr, 32 )
        ,strbuff);
        NetTool::Csendto(server.getsockFD(),"so sex",7,0,(((struct sockaddr*)(&clintaddr))),clen);
        sleep(1);
    }
    return 0;
}
