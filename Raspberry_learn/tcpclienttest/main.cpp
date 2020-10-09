#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP client\n");
    TcpClient client;
    client.setAddr(INADDR_ANY);
    client.setFamily(AF_INET);
    client.setPort(40960);
    if(!client.startConnect())
    {
        LOG_ERROR("start Tcp Server failuer!");
        return -1;
    }
    CHAR* buff[512] = {0};
    read(client.getSockfd(),buff, 512); 
    LOG_INFO("GET MSG FROM SERVER: %s\n",buff);
    return 0;
}
