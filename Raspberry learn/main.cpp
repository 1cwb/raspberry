#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "msg.h"
#include "condition.h"
#include "thread.h"
#include "usbEvent.h"
#include "cfileSys.h"
#include "oled.h"
#include "fonts.h"
#include "cfile.h"
#define MAX_MSG_QUEUE_SIZE 1024

Msg msg_test((char*)"queuetest",MAX_MSG_QUEUE_SIZE,1);
Msg msg_oled((char*)"msg_oled",MAX_MSG_QUEUE_SIZE,1);

CfileSys test;

void* usbEvendParse(VOID* p)
{
    INT32 num = -1;
    INT32 state = -1;
    
    cntl_queue_ret msg_cnt = CNTL_QUEUE_INVALIED;
    queue_buf *data1 = NULL;
    queue_buf *data_oled = NULL;
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
        data_oled = (queue_buf*)malloc(sizeof(queue_buf));
        if(data_oled != NULL)
        {
            memcpy(data_oled, data1, sizeof(queue_buf));
            msg_oled.push(data_oled, 0, IPC_BLOCK);
        }     
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
                            data_oled = (queue_buf*)malloc(sizeof(queue_buf));
                            if(data_oled != NULL)
                            {
                                msg_oled.formatMsg(data_oled, "Write Msg to Udisk pass", 
                                            strlen("Write Msg to Udisk pass") + 1, MSG_WRITE_MSG_TO_DISK_OK);
                                msg_oled.push(data_oled, 1, IPC_WAITTIMES);
                            }
                        }
                    }
                    //printf("%s\n",buff);
                    free(buff);
					
                    queue_buf* oled_update_ip = (queue_buf*)malloc(sizeof(queue_buf));
                    msg_oled.formatMsg(oled_update_ip, "", 1, MSG_UPDATE_IP);
                    msg_oled.push(oled_update_ip, 0, IPC_BLOCK);
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
                        data_oled = (queue_buf*)malloc(sizeof(queue_buf));
                        if(data_oled != NULL)
                        {
                            msg_oled.formatMsg(data_oled, "A normal Udisk", 
                                        strlen("A normal Udisk") + 1, MSG_WRITE_MSG_TO_DISK_FAIL);
                            msg_oled.push(data_oled, 0, IPC_BLOCK);
                        }    
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

INT32 main()
{
    //fork test
    pid_t pid = Cfork();
    if(pid == 0)
    {
        DBG("i am son, my pid is %d", (INT32)Cgetpid());
        sleep(5);
        C_Exit(0);
       // C_exit(0);
      //  Cexit(0);
    }
    DBG("son pid is %d",(INT32)pid);
    siginfo_t siginfo;
    memset(&siginfo,0,sizeof(siginfo));
    DBG("father wait son exit");
    DBG("son exit %d, %s", Cwaitid(P_PID, pid, &siginfo, WEXITED), Cstrerror(getErrno()));

    DBG("%s",Cgetlogin());
    struct passwd* pwd = Cgetpwuid(Cgetuid());
    DBG("uid is %d",Cgetuid());
    DBG("username is %s,passwd is %s,home dir is %s,shell is %s,user information is %s,uid is %d",pwd->pw_name,pwd->pw_passwd,pwd->pw_dir,pwd->pw_shell,pwd->pw_gecos,pwd->pw_uid);

    queue_buf *oled_data = NULL;
    CHAR myTime[32] = {0};
    INT32 UpdataIp = 0;
    INT32 defaultTime = 15;
    DBG("Raspberry Service Running!!!");
    OledRun oled(&msg_oled);
    oled.run();
    Thread th_usb_event_listen;
    Thread th_usb_event_parse;
    th_usb_event_listen.setThreadFunc(usbEvendListen, NULL);
    th_usb_event_parse.setThreadFunc(usbEvendParse, NULL);
    th_usb_event_listen.start();
    th_usb_event_parse.start();

    while(true)
    {
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

    }
    msg_test.setMsgCancel();
    th_usb_event_listen.join();
    th_usb_event_parse.join();
	return 0;
}
