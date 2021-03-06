#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "clog.h"
#include "msocket.h"
#include "cfile.h"
#include "cselect.h"
#include "cfileSys.h"
#include "cfile.h"
NetClient* mclient[1000000];
int main(INT32 argc, CHAR** argv)
{
    Clog::getInstance()->Init(STDOUT_FILENO,LEVEL_DEBUG);
    NetTool:: getAddrTest();
    Cfile file(stdin);
    for(UINT32 i = 0; i  < 1000000; i++)
    {
        mclient[i] = new NetClient("192.168.31.28","40960");
        if(!mclient[i]->initAddr(AF_INET,0,0,AI_CANONNAME))
        {
            LOG_ERROR("init client failer!");
        }
        if(!mclient[i]->startConnect())
        {
            LOG_ERROR("connect server failer!");
        }
        LOG_DEBUG("client num is %u",i);
    }
/*
    Cselect mselect;
    LOG_INFO("Now Start a TCP client");
    NetClient client("192.168.31.28","40960");
    if(!client.initAddr(AF_INET,0,0,AI_CANONNAME))
    {
        LOG_ERROR("init client failer!");
        return -1;
    }
    if(!client.startConnect())
    {
        LOG_ERROR("connect server failer!");
        return -1;
    }*/
/*
    LOG_INFO("connect successful!");
    NetTool::Csetfdblock(client.getsockFD(), false);
    INT32 selectNum = client.getsockFD();
    while(true)
    {
        mselect.fdSet(file.getFILEno(), READ_FD_EM);
        mselect.fdSet(client.getsockFD(), READ_FD_EM);
        mselect.selectfd(selectNum + 1, NULL);
        if(mselect.fdIsSet(client.getsockFD(), READ_FD_EM))
        {
            CHAR recvbuff[1024] = {0};
            INT32 n = recv(client.getsockFD(),recvbuff, 1024, 0);//read(client.getsockFD(),recvbuff, 512);
            if(n < 0)
            {
                LOG_ERROR("READ ERROR!!!!");
            }
            else if(n > 0)
            {
                LOG_INFO("%s",recvbuff);
            }
            else
            {
                client.CcloseSockfd();
                break;
            }
            
        }
        if(mselect.fdIsSet(file.getFILEno(), READ_FD_EM))
        {
            CHAR sendbuff[1024] = {0};
            file.Cfgets(sendbuff,1024);
            file.Cfflush();
            send(client.getsockFD(), sendbuff, 1024, 0);
        }
    }*/
    //file.Cfclose();
    return 0;
}
