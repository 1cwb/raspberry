#include "threadtask.h"
#include "usbEvent.h"
#include "cfileSys.h"
#include "cfile.h"
#include "thread.h"
#include "msg.h"


#ifdef OLED_DRIVER_ON
#include "oled.h"
#endif

#ifdef OLED_DRIVER_ON
extern Msg msg_oled;
#endif
extern Msg msg_test;


VOID* parseIniFile(VOID* p)
{
    if(p == NULL)
    {
        return NULL;
    }
    INT32 nStart = 0;
    INT32 nEnd = 0;
    CHAR fileName[128] = {0};
    CHAR resultFileName[128] = {0};
    CHAR cmdBuff[128] = {0};
    CHAR* resultBuff = (CHAR*)malloc(2098);
 
    CMD_TYPE dataTYpe = RASPBERRY_INVAILED_EM;
    strcat(fileName, (CHAR*) p);
    strcat(fileName,RASPBERRY_COMMAND);
    strcat(resultFileName, (CHAR*) p);
    strcat(resultFileName,RASPBERRY_IP_FILE);
    if(Caccess(fileName, F_OK) == 0)
    {
        DBG("OPEN CMD FILE %s",fileName);
        DBG("OPEN RESULT FILE %s",resultFileName);
        Cfile raspFile(fileName, "rb");
        Cfile resutFile(resultFileName, "ab+");
        DBG("%s",Cstrerror(getErrno()));
        if(raspFile.isFileopenSuccess())
        {
            while(raspFile.Cfgets(cmdBuff,sizeof(cmdBuff)) != NULL && !raspFile.Cfeof())
            {
                if(FindPattern(cmdBuff, "^[ ]*#", nStart, nEnd))
                {
                    //DBG("%s",cmdBuff);
                    continue;
                }
                RemoveCRLF(cmdBuff);
                if(strncmp(cmdBuff,RASPBERRY_SYS_CMD, strlen(RASPBERRY_SYS_CMD)) == 0)
                {
                    dataTYpe = RASPBERRY_SYS_CMD_EM;
                    memset(cmdBuff, 0, sizeof(cmdBuff));
                    continue;
                }
                else if(strncmp(cmdBuff,RASPBERRY_UPDATE_VALUE, strlen(RASPBERRY_UPDATE_VALUE)) == 0)
                {
                    dataTYpe = RASPBERRY_UPDATE_VALUE_EM;
                    memset(cmdBuff, 0, sizeof(cmdBuff));
                    continue;
                }
                
                switch(dataTYpe)
                {
                    case RASPBERRY_SYS_CMD_EM:
                        DBG("RUN SYSTEM COMMAND");  
                        if(!DoComman(cmdBuff, "r", resultBuff, 2098))
                        {
                            DBG("%s",Cstrerror(getErrno()));
                        }
                        if(resutFile.isFileopenSuccess())
                        {
                            resutFile.Cfwrite(resultBuff, 1, strlen(resultBuff));
                        }
                        memset(resultBuff, 0, sizeof(CHAR)*2098);
                        break;
                    case RASPBERRY_UPDATE_VALUE_EM:
                        DBG("CASE 2");
                        break;
                    default: break;
                }
                memset(cmdBuff, 0, sizeof(cmdBuff));
            }
        }
        raspFile.Cfclose();
        resutFile.Cfclose();
    }  
    #ifdef OLED_DRIVER_ON
    queue_buf *data_oled = NULL;
    data_oled = (queue_buf*)malloc(sizeof(queue_buf));
    if(data_oled != NULL)
    {
        msg_oled.formatMsg(data_oled, "Write Msg to Udisk pass", 
                    strlen("Write Msg to Udisk pass") + 1, MSG_WRITE_MSG_TO_DISK_OK);
        msg_oled.push(data_oled, 1, IPC_WAITTIMES);
    }
    #endif
    free(resultBuff);
    return NULL;
}
VOID* usbEvendParse(VOID* p)
{
    INT32 state = -1;
    
    cntl_queue_ret msg_cnt = CNTL_QUEUE_INVALIED;
    queue_buf *data1 = NULL;
    #ifdef OLED_DRIVER_ON
    queue_buf *data_oled = NULL;
    #endif
    CHAR filepatch[FILE_PATCH_LEN] = {0};
    while(msg_cnt != CNTL_QUEUE_CANCEL)
    {
        msg_cnt = msg_test.pop((VOID**)&data1,0, IPC_BLOCK);
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
                strcat(filepatch,data1->msg_buf);
                strcat(filepatch,RASPBERRY_PATH);
                //DBG("path name is %s",filepatch);
                state = Caccess(filepatch, F_OK);
                if(state == 0)
                {
                    Thread parseIni;
                    parseIni.setThreadFunc(parseIniFile,filepatch);
                    parseIni.start();
                #ifdef OLED_DRIVER_ON
                    queue_buf* oled_update_ip = (queue_buf*)malloc(sizeof(queue_buf));
                    msg_oled.formatMsg(oled_update_ip, "", 1, MSG_UPDATE_IP);
                    msg_oled.push(oled_update_ip, 0, IPC_BLOCK);
                #endif
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
    return NULL;
}

VOID* usbEvendListen(VOID* p)
{
    usbEvent uevent;
    queue_buf *data = NULL;
    CHAR usb_patch_name[USB_PATCH_NAME_LEN] = {0};
    CHAR *buf = (char*)malloc(sizeof(CHAR) * UEVENT_BUFFER_SIZE);
    while(true)
    {
        memset(buf, 0, UEVENT_BUFFER_SIZE * sizeof(CHAR));
        memset(usb_patch_name, 0, USB_PATCH_NAME_LEN * sizeof(CHAR));
		recv(uevent.getSockid(), buf, UEVENT_BUFFER_SIZE, 0);
		MSG_EVENT event = uevent.getUsbEvent(buf, usb_patch_name, USB_PATCH_NAME_LEN);
        data = (queue_buf*)malloc(sizeof(queue_buf));
        switch (event)
        {
            case MSG_USB_ADD:
                msg_test.formatMsg(data, usb_patch_name, strlen(usb_patch_name) + 1, event);
                msg_test.push(data, 0, IPC_BLOCK);
                uevent.showAllUsbPath();
                break;
            case MSG_USB_NOT_MOUNTED:
                DBG("usb incerted but not mounted");
                break;
            case MSG_USB_REMOVE:
                msg_test.formatMsg(data, "UsbEvent:usb remove!", strlen("UsbEvent:usb remove!") + 1, event); 
	            msg_test.push(data, 0, IPC_BLOCK);
                uevent.showAllUsbPath();
                break;
            default: break;
        }
    }
    free(buf);
    buf = NULL;
    return NULL;
}

