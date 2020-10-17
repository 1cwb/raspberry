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
 
    TcpClient client(AF_INET);
    if(!client.initAddr(40960, "192.168.31.6"))
    {
        LOG_ERROR("init client failer!");
        return -1;
    }
    if(!client.startConnect())
    {
        LOG_ERROR("connect server failer!");
        return -1;
    }
    while(true)
    {
    CHAR buff[256] = {0};
    memset(buff, 0, 256);
    INT32 n = read(client.getsockFD(),buff, 256); 
    printf("GET MSG FROM SERVER: %s\n",buff);
    if(n == 0)break;
    }
    return 0;
}
