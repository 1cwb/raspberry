#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "cfile.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_INFO("Now Start a TCP client");
 
    NetClient client(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(!client.initAddr(40960, "192.168.31.28"))
    {
        LOG_ERROR("init client failer!");
        return -1;
    }
    /*if(!client.startConnect())
    {
        LOG_ERROR("connect server failer!");
        return -1;
    }*/
    //INT32 connfd = -1;
    CHAR strbuff[256];
    CHAR straddr[128] = {0};
    while(true)
    {
        struct sockaddr_storage clintaddr;
        socklen_t clen = sizeof(struct sockaddr_storage);
        memset(&clintaddr, 0, clen);
        memset(strbuff,0,256);
        memset(straddr,0,128);
        NetTool::Csendto(client.getsockFD(),"caonima B",10,0,client.getSockaddr(),client.getSockaddrLen());
        //write(client.getsockFD(), "sbbbbb", 7);
        NetTool::Crecvfrom(client.getsockFD(),strbuff,256,0,(((struct sockaddr*)(&clintaddr))),&clen);
        LOG_DEBUG("recieve from %s is %s",
        NetTool::CinetNtop(clintaddr.ss_family, &(((struct sockaddr_in6*)(&clintaddr))->sin6_addr),straddr, 32 )
        ,strbuff);
        
    }
    return 0;
}
