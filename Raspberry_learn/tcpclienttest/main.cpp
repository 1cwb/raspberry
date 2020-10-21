#include <unistd.h>
#include <string.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "cfile.h"
#include "cselect.h"
#include "cfile.h"
int main()
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    NetTool:: getAddrTest();
    Cfile file(STDIN_FILENO);
    Cselect mselect;
    
    LOG_INFO("Now Start a TCP client");
    NetClient client("192.168.31.6","40960");
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
    mselect.fdSet(STDIN_FILENO, READ_FD_EM);
    mselect.fdSet(client.getsockFD(), READ_FD_EM);
    INT32 selectNum = client.getsockFD() + 1;
    while(true)
    {
        if(mselect.selectfd(selectNum, NULL) < 0)
        {
            continue;
        }
        DBG("CAN WRITE");
        for(INT32 fd = 0; fd < selectNum; fd ++)
        {
            if(mselect.fdIsSet(fd, READ_FD_EM))
            {
                if(fd == STDIN_FILENO)
                {
                    LOG_INFO("write to server!!!");
                    CHAR sendBUff[512] = {0};
                    file.Cfgets(sendBUff, 512);
                    write(fd, sendBUff, 512);
                }
                else
                {
                    CHAR buff[512] = {0};
                    memset(buff, 0, 256);
                    INT32 n = read(client.getsockFD(),buff, 256); if(n == 0)continue;
                    LOG_INFO("%s\n",buff);
                }
                mselect.fdSet(fd, READ_FD_EM);
            }
        }
    
    }
    //file.Cfclose();
    return 0;
}
