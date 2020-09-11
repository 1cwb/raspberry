#include "threadtask.h"
#include "usbEvent.h"
#include "cfileSys.h"
#include "cfile.h"
#include "thread.h"
#include "msg.h"
#include "clog.h"
#include "cepoll.h"

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
    CHAR* tempBuff = (CHAR*)malloc(512);
    CHAR* resultBuff = (CHAR*)malloc(2048);

    CMD_TYPE dataTYpe = RASPBERRY_INVAILED_EM;
    PRIVATE_CMD_TYPE private_type = RASPBERRY_UPDATE_VALUE_INVALIED_EM; 
    
    memset(tempBuff, 0, 512);
    memset(resultBuff, 0, 2048);
    strcat(fileName, (CHAR*) p);
    strcat(fileName,RASPBERRY_COMMAND);
    strcat(resultFileName, (CHAR*) p);
    strcat(resultFileName,RASPBERRY_IP_FILE);
    if(Caccess(fileName, F_OK) == 0)
    {
        LOG_DEBUG("OPEN CMD FILE %s",fileName);
        LOG_DEBUG("OPEN RESULT FILE %s",resultFileName);
        Cfile raspFile(fileName, "rb");
        Cfile resutFile(resultFileName, "ab+");
        if(raspFile.isFileopenSuccess())
        {
            while(!raspFile.Cfeof() && raspFile.Cfgets(cmdBuff, 128) != NULL)
            {
                if(FindPattern(cmdBuff, "^[ ]*#", nStart, nEnd))
                {
                    LOG_DEBUG("main cmd class %s",cmdBuff);
                    continue;
                }
                RemoveCRLF(cmdBuff);
		        LOG_DEBUG("parse cmd line %s",cmdBuff);
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
                        LOG_DEBUG("RUN SYSTEM COMMAND");  
                        if(!DoComman(cmdBuff, "r", resultBuff, 2048))
                        {
                            LOG_ERROR("%s",Cstrerror(Cgeterrno()));
                        }
                        if(resutFile.isFileopenSuccess())
                        {
                            resutFile.Cfwrite(resultBuff, 1, strlen(resultBuff));
                        }
                        memset(resultBuff, 0, sizeof(CHAR)*2048);
                        break;
                    case RASPBERRY_UPDATE_VALUE_EM:
                        LOG_DEBUG("get private cmd: %s",cmdBuff);
                        if(strncmp(cmdBuff, RASPBERRY_UPDATE_VALUE_ADD_WIFI, strlen(RASPBERRY_UPDATE_VALUE_ADD_WIFI)) == 0)
			            {
			                private_type = RASPBERRY_UPDATE_VALUE_ADD_WIFI_EM;
			                break;
			            }
                        switch(private_type)
			            {
			                case RASPBERRY_UPDATE_VALUE_ADD_WIFI_EM:
				                if(strstr(cmdBuff, "ssid") != NULL)
				                {
			                        strcat(tempBuff,"\nnetwork={\n");
				                    strcat(tempBuff,cmdBuff);
				                    strcat(tempBuff,"\n");
				                }
				                if(strstr(cmdBuff, "psk") != NULL)
				                {
				                    strcat(tempBuff, cmdBuff);
				                    strcat(tempBuff, "\n");
				                    strcat(tempBuff,"}\n");
				                    LOG_DEBUG("%s",tempBuff);
				                    LOG_DEBUG("open file %s",WPA_SUPPLICANT_CONFIG);
				                    Cfile wpa_file(WPA_SUPPLICANT_CONFIG, "a");
				                    if(wpa_file.Cfwrite(tempBuff, strlen(tempBuff), 1) < 0)
				                    {
				                        LOG_ERROR("Add wifi failer!");
					                    LOG_ERROR("Error is %s",Cstrerror(Cgeterrno()));
				                    }
				                     private_type = RASPBERRY_UPDATE_VALUE_INVALIED_EM;
                                     wpa_file.Cfclose();
				                }
			                    break;
			                default:
				            break;
			            }
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
    free(tempBuff);
    tempBuff = NULL;
    free(resultBuff);
    resultBuff = NULL;
    return NULL;
}
VOID* usbEvendParse(VOID* p)
{
    INT32 state = -1;
    Cepoll mepoll(1);
    struct epoll_event ctlevent;
    struct epoll_event *events;
   // mepoll.CepollCtl
    cntl_queue_ret msg_cnt = CNTL_QUEUE_INVALIED;
    queue_buf *data1 = NULL;
    #ifdef OLED_DRIVER_ON
    queue_buf *data_oled = NULL;
    #endif
    CHAR filepatch[FILE_PATCH_LEN] = {0};
    events = (epoll_event*)malloc(sizeof(struct epoll_event) * 1024);
    if(events == NULL)
    {
        LOG_ERROR("Malloc event failer!");
        abort();
    }
    while(msg_cnt != CNTL_QUEUE_CANCEL)
    {
        msg_cnt = msg_test.pop((VOID**)&data1,0, IPC_BLOCK);
        memset(filepatch, 0, FILE_PATCH_LEN);
	
        if(data1 == NULL)
        {
            LOG_DEBUG("in main data  === NULL");
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
                LOG_INFO("Get Msg: data->type is %d, data->msg_buf is %s\n",data1->msg_type,data1->msg_buf);
                strcat(filepatch,data1->msg_buf);
                strcat(filepatch,RASPBERRY_PATH);
                LOG_DEBUG("path name is %s",filepatch);
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
                    //get a normal usb, create RASPBERRY_COMMAND FILE
                    if(Cmkdir(filepatch) == 0)
                    {
                        strcat(filepatch, RASPBERRY_COMMAND);
                        Cfile cmdfile(filepatch);
                        cmdfile.Cfwrite(RASPBERRY_SYS_CMD"\n", strlen(RASPBERRY_SYS_CMD"\n"), 1);
                        cmdfile.Cfwrite(RASPBERRY_UPDATE_VALUE"\n", strlen(RASPBERRY_UPDATE_VALUE"\n"), 1);
                        cmdfile.Cfclose();
                    }
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
                LOG_INFO("Get Msg: data->type is %d, data->msg_buf is %s\n",data1->msg_type,data1->msg_buf);
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
                LOG_INFO("usb incerted but not mounted");
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

