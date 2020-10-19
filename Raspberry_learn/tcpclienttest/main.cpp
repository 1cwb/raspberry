#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "cfile.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    NetTool:: getAddrTest();
    LOG_INFO("Now Start a TCP client");
    NetClient client("www.google.com","http");
    if(!client.initAddr(AF_INET,0,0,AI_CANONNAME))
    {
        LOG_ERROR("init client failer!");
        return -1;
    }
    if(!client.startConnect())
    {
        LOG_ERROR("connect server failer!");
        return -1;
    }
    LOG_INFO("connect successful!");
    while(true)
    {
    CHAR buff[256] = {0};
    memset(buff, 0, 256);
    INT32 n = read(client.getsockFD(),buff, 256); if(n == 0)break;
    LOG_INFO("GET MSG FROM SERVER: %s\n",buff);
    
    }
    return 0;
}
