#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
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
    INT32 connfd;
    //CHAR sendBuff[1024] = {0};
    while(true)
    {
        connfd = server.accept(NULL, NULL);
        if(connfd >= 0)
        {
            write(connfd,"hello world!", sizeof("hello world!"));
            close(connfd);
        }
    }
    return 0;
}
