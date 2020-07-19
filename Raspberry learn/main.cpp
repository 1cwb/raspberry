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
#include "cfile.h"
#include "csignal.h"
#ifdef OLED_DRIVER_ON
#include "oled.h"
#endif
#define MAX_MSG_QUEUE_SIZE 1024

Msg msg_test((char*)"queuetest",MAX_MSG_QUEUE_SIZE,1);
#ifdef OLED_DRIVER_ON
Msg msg_oled((char*)"msg_oled",MAX_MSG_QUEUE_SIZE,1);
#endif

CfileSys test;

void* usbEvendParse(VOID* p)
{
    INT32 num = -1;
    INT32 state = -1;
    
    cntl_queue_ret msg_cnt = CNTL_QUEUE_INVALIED;
    queue_buf *data1 = NULL;
    #ifdef OLED_DRIVER_ON
    queue_buf *data_oled = NULL;
    #endif
    CHAR *filepatch = (CHAR*)malloc(sizeof(CHAR) * FILE_PATCH_LEN);
    struct dirent *file = (struct dirent *)malloc(sizeof(struct dirent) * DIRECTORY_NUM);
    while(msg_cnt != CNTL_QUEUE_CANCEL)
    {
        msg_cnt = msg_test.pop((VOID**)&data1,0, IPC_BLOCK);
        memset(file, 0, sizeof(struct dirent) * DIRECTORY_NUM);
        memset(filepatch, 0, FILE_PATCH_LEN);
	
        if(data1 == NULL)
        {
            //DBG("in main data  === NULL");
            continue;
        }
    //for oled
    #ifdef OLED_DRIVER_ON
        data_oled = (queue_buf*)malloc(sizeof(queue_buf));
        if(data_oled != NULL)
        {
            memcpy(data_oled, data1, sizeof(queue_buf));
            msg_oled.push(data_oled, 0, IPC_BLOCK);
        }
    #endif
        switch(data1->msg_type)
        {
            case MSG_USB_ADD:
                DBG("Get Msg: data->type is %d, data->msg_buf is %s\n",data1->msg_type,data1->msg_buf);
                sleep(1);
                num = usbEvent::getUsbName("/media/pi/", file,DIRECTORY_NUM);
                state = usbEvent::getRaspCmdFile(file, num, filepatch, FILE_PATCH_LEN);
                if(state == 0)
                {
                    CHAR* buff = (char*)malloc(2098);
                    DoComman("ifconfig", "r", buff, 2098);
                    Cfile raspFile(filepatch, "w");
                    if(raspFile.isFileopenSuccess())
                    {
                        if(raspFile.Cfwrite(buff, 1, strlen(buff)) > 0)
                        {
                        #ifdef OLED_DRIVER_ON
                            data_oled = (queue_buf*)malloc(sizeof(queue_buf));
                            if(data_oled != NULL)
                            {
                                msg_oled.formatMsg(data_oled, "Write Msg to Udisk pass", 
                                            strlen("Write Msg to Udisk pass") + 1, MSG_WRITE_MSG_TO_DISK_OK);
                                msg_oled.push(data_oled, 1, IPC_WAITTIMES);
                            }
                        #endif
                        }
                    }
                    //printf("%s\n",buff);
                    free(buff);
                #ifdef OLED_DRIVER_ON
                    queue_buf* oled_update_ip = (queue_buf*)malloc(sizeof(queue_buf));
                    msg_oled.formatMsg(oled_update_ip, "", 1, MSG_UPDATE_IP);
                    msg_oled.push(oled_update_ip, 0, IPC_BLOCK);
                #endif
                    /*Parse_val_t *param = (Parse_val_t*)malloc(sizeof(Parse_val_t) * 100);
                    if(param == NULL)
                    {
                        DBG("error: param is NULL point!");
                        break;
                    }
                    memset(param, 0, sizeof(Parse_val_t) * 100);
                    usbEvent::parseRaspFile(filepatch, param, 100);
                    for(i = 0;i < 100; i++)
                    {
                        if((param + i)->bused == 1)
                        {
                            printf("param is %s\n",(param + i)->param);
                            printf("key is %d\n",(param + i)->type);
                        }
                        else
                        {
                            break;
                        }
                    }
                    free(param);
                    param = NULL;*/
                }
                else
                {
                    #ifdef OLED_DRIVER_ON
                    data_oled = (queue_buf*)malloc(sizeof(queue_buf));
                    if(data_oled != NULL)
                    {
                        msg_oled.formatMsg(data_oled, "A normal Udisk", 
                                    strlen("A normal Udisk") + 1, MSG_WRITE_MSG_TO_DISK_FAIL);
                        msg_oled.push(data_oled, 0, IPC_BLOCK);
                    }
                    #endif
                }
                break;
            case MSG_USB_REMOVE:
                printf("Get Msg: data->type is %d, data->msg_buf is %s\n",data1->msg_type,data1->msg_buf);
                break;
            default: break;
        }
        free(data1);
        data1 = NULL;
    }
    free(filepatch);
    free(file);
    filepatch = NULL;
    file = NULL;
    return NULL;
}

VOID* usbEvendListen(VOID* p)
{
    usbEvent uevent;
    queue_buf *data = NULL;

    CHAR *buf = (char*)malloc(sizeof(CHAR) * UEVENT_BUFFER_SIZE);
    while(true)
    {
        memset(buf, 0, UEVENT_BUFFER_SIZE * sizeof(CHAR));
		recv(uevent.getSockid(), buf, UEVENT_BUFFER_SIZE, 0);
		MSG_EVENT event = uevent.getUsbEvent(buf);
        DBG("%s",buf);
        data = (queue_buf*)malloc(sizeof(queue_buf));
        if(event == MSG_USB_ADD)
        {
            msg_test.formatMsg(data, "UsbEvent:usb add!", strlen("UsbEvent:usb add!") + 1, event);
	        msg_test.push(data, 0, IPC_BLOCK);
        }
        if(event == MSG_USB_REMOVE)
        {
            msg_test.formatMsg(data, "UsbEvent:usb remove!", strlen("UsbEvent:usb remove!") + 1, event); 
	        msg_test.push(data, 0, IPC_BLOCK);
	    }
    }
    free(buf);
    buf = NULL;
    return NULL;
}

VOID sigtestfunc(INT32 sig)
{
    DBG("get signal %d",sig);
}

INT32 main()
{
    DBG("my PID is %d",Cgetpid());
    DBG("Raspberry Service Running!!!");
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
    #endif
        DBG("Main thread pause!");
        pause();
    }
    msg_test.setMsgCancel();
#ifdef OLED_DRIVER_ON
    msg_oled.setMsgCancel();
#endif
    th_usb_event_listen.join();
    th_usb_event_parse.join();
	return 0;
}
