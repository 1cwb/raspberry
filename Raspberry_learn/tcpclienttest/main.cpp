#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "cfile.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    LOG_DEBUG("Now Start a TCP client\n");
    TcpClient client;
    client.setAddr("INADDR_ANY");
    client.setFamily(AF_INET);
    client.setPort(40960);
    if(!client.startConnect())
    {
        LOG_ERROR("start Tcp client failuer!");
        return -1;
    }
    else{
        Cfile file(STDOUT_FILENO);
    }
    CHAR* buff[512] = {0};
    int n = 0;
    while(true)
    {
        memset(buff, 0, 512);
        write(client.getSockfd(),"wocao!!!",sizeof("wocao!!!"));
    n = read(client.getSockfd(),buff, 512); 
    printf("GET MSG FROM SERVER: %s\n",buff);
    if(n == 0)break;
    }
    return 0;
}
