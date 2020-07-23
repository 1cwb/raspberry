#include "usbEvent.h"
#include "common.h"
#include "cfile.h"
#include "deelx.h"

usbEvent::usbEvent() : sockid(-1)
{
    sockid = initHotplugSock();
    if(sockid < 0)
    {
        printf("create usbEvent fail\n");
    }
	memset(usb_path_info, 0, sizeof(usb_path_info));
}
usbEvent::~usbEvent()
{
    close(sockid);
}
bool usbEvent::addUsb(const CHAR* dev_name, const CHAR* path_name)
{
	for(INT32 i = 0; i < MAX_USB_DEV_NUM; i++)
	{
		if(usb_path_info[i].beused == false)
		{
			memcpy(usb_path_info[i].dev, dev_name, strlen(dev_name) + 1);
			memcpy(usb_path_info[i].path_name, path_name, strlen(path_name) + 1);
			usb_path_info[i].beused = true;
			return true;
		}
	}
	DBG("Err: there is no enough space to save usb dev");
	return false;
}
bool usbEvent::removeUsb(const CHAR* dev_name)
{
    for(INT32 i = 0; i < MAX_USB_DEV_NUM; i++)
	{
		if((usb_path_info[i].beused == true) && (strcmp(dev_name,usb_path_info[i].dev) == 0))
		{
            memset(&usb_path_info[i], 0, sizeof(usb_device_t));
			usb_path_info[i].beused = false;
			DBG("Remove %s successful",dev_name);
			return true;
		}
	}
	DBG("Err: remove %s fail, can not find it!",dev_name);
	return false;
}
VOID usbEvent::showAllUsbPath()
{
	for(INT32 i = 0; i < MAX_USB_DEV_NUM; i++)
	{
		if(usb_path_info[i].beused)
		{
           printf("Usb Dev is %s, path name is %s\n",usb_path_info[i].dev,usb_path_info[i].path_name);
		}
	}
}
MSG_EVENT usbEvent::getUsbEvent(const CHAR* buf, CHAR* usb_patch_name, INT32 len)
{
	INT32 nStart = 0;
	INT32 nEnd   = 0;
	Cfile mfile;
	CHAR file_line[FILE_LINE_LEN] = {0};
	CHAR usb_data[USB_SDX_LEN] = {0};
	MSG_EVENT event = MSG_INVALIED;
	if(len < USB_PATCH_NAME_LEN)
	{
	    DBG("Error: usb_data len is short than 128!");
	    return event;
	}
	if(strstr(buf,"add")!= NULL)
	{
            if(FindPattern(buf, "sd[a-z][0-9]", nStart, nEnd))
	    {
                memcpy(usb_data ,buf + nStart, nEnd - nStart);
                usb_data[nEnd] = '\0';
		sleep(1);
                sync();
                if(mfile.Cfopen(USB_MOUNT_INFO, "rb"))
		{
		    while(mfile.Cfgets(file_line, sizeof(file_line)) != NULL && !mfile.Cfeof())
		    {
			//DBG("%s",file_line);
                        if(FindPattern(file_line, usb_data, nStart, nEnd))
			{
                            if(FindPattern(file_line, " /[\\S/]+ ", nStart, nEnd))
			    {
                                memcpy(usb_patch_name ,file_line + nStart + 1, nEnd - nStart - 2);
			        usb_patch_name[nEnd - 2] = '\0';
				addUsb(usb_data, usb_patch_name);
				//DBG("add usb[%s] add usb patch name is %s",usb_data,usb_patch_name);
                                event = MSG_USB_ADD;
				break;
			    }
			}
			memset(file_line, FILE_LINE_LEN * sizeof(CHAR), sizeof(file_line));
		    }
		}
		mfile.Cfclose();
                return event == MSG_USB_ADD ? MSG_USB_ADD : MSG_USB_NOT_MOUNTED;
	    }
	}
	else if(strstr(buf,"remove")!= NULL)
	{
            if(FindPattern(buf, "sd[a-z][0-9]", nStart, nEnd))
	    {
		memcpy(usb_data ,buf + nStart, nEnd - nStart);
		usb_data[nEnd] = '\0';
		removeUsb(usb_data);
                //printf("remove usb[%s] now!!!\n",usb_data);
                return MSG_USB_REMOVE;
	    }
	}
	return MSG_UNKNOW_EVENT;
}
INT32 usbEvent::initHotplugSock(void)
{
    struct sockaddr_nl snl;
    const INT32 buffersize = 1024;
    INT32 retval;
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;
    INT32 hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (hotplug_sock == -1)
    {
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }
    /* set receive buffersize */
    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));
    retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
    if (retval < 0) 
	{
        printf("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }
    return hotplug_sock;

}

INT32 usbEvent::getSockid()
{
    return sockid;
}

