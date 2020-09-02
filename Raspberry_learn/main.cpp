#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "timemeasure.h"
#include "msg.h"
#include "condition.h"
#include "thread.h"
#include "usbEvent.h"
#include "cfileSys.h"
#include "fonts.h"
#include "csignal.h"
#include "threadtask.h"
#include "clog.h"
#ifdef OLED_DRIVER_ON
#include "oled.h"
#endif
#define MAX_MSG_QUEUE_SIZE 1024

Msg msg_test((char*)"queuetest",MAX_MSG_QUEUE_SIZE,1);
#ifdef OLED_DRIVER_ON
Msg msg_oled((char*)"msg_oled",MAX_MSG_QUEUE_SIZE,1);
#endif

INT32 main(INT32 argc, CHAR** argv)
{
    DBG("my PID is %d",Cgetpid());
    DBG("Raspberry Service Running!!!");
#ifdef RUN_AS_DAEMON
    Daemonize(argv[0]);
    Clog::getInstance()->Init("/home/daemeon.log",0);
#else
    Clog::getInstance()->Init(STDOUT_FILENO,0);
#endif
#ifdef OLED_DRIVER_ON
    queue_buf *oled_data = NULL;
    CHAR myTime[32] = {0};
    INT32 UpdataIp = 0;
    INT32 defaultTime = 15;
    OledRun oled(&msg_oled);
    oled.run();
#endif
    Thread th_usb_event_listen;
    Thread th_usb_event_parse;
    th_usb_event_listen.setThreadFunc(usbEvendListen, NULL);
    th_usb_event_parse.setThreadFunc(usbEvendParse, NULL);
    th_usb_event_listen.start();
    th_usb_event_parse.start();

    while(true)
    {
    #ifdef OLED_DRIVER_ON
        if(UpdataIp > defaultTime)
        {
            UpdataIp = 0;
            defaultTime = 6000;
            oled_data = (queue_buf*)malloc(sizeof(queue_buf));
            msg_oled.formatMsg(oled_data, myTime, strlen(myTime) + 1, MSG_UPDATE_IP);
            msg_oled.push(oled_data, 0, IPC_BLOCK);
        }
        oled_data = (queue_buf*)malloc(sizeof(queue_buf));
        memset(myTime, 0, sizeof(myTime));
        getLocalTime(myTime, sizeof(myTime));
        msg_oled.formatMsg(oled_data, myTime, strlen(myTime) + 1, MSG_SHOW_TIME);
        msg_oled.push(oled_data, 0, IPC_BLOCK);

        sleep(1);
        UpdataIp ++;
    #else
        DBG("Main thread pause!");
        pause();
    #endif
    }
    msg_test.setMsgCancel();
#ifdef OLED_DRIVER_ON
    msg_oled.setMsgCancel();
#endif
	return 0;
}
