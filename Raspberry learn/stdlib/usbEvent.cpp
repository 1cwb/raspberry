#include "usbEvent.h"
#include "common.h"
#include "deelx.h"

usbEvent::usbEvent() : sockid(-1)
{
    sockid = initHotplugSock();
    if(sockid < 0)
    {
        printf("create usbEvent fail\n");
    }
}
usbEvent::~usbEvent()
{
    close(sockid);
}
MSG_EVENT usbEvent::getUsbEvent(const char* buf)
{
	const CHAR* p = NULL;
	if((p = strstr(buf,"add"))!= NULL)
	{
		if(((p = strstr(buf,"sda/sda"))!= NULL) || 
			((p = strstr(buf,"sdb/sdb"))!= NULL) || 
				((p = strstr(buf,"sdc/sdc"))!= NULL) || 
					((p = strstr(buf,"sdd/sdd"))!= NULL))
		{
			printf("add usb now!!!\n");
			return MSG_USB_ADD;
		}
	}
	else if((p = strstr(buf,"remove"))!= NULL)
	{
		if(((p = strstr(buf,"sda/sda"))!= NULL) ||
			((p = strstr(buf,"sdb/sdb"))!= NULL) ||
				((p = strstr(buf,"sdc/sdc"))!= NULL) ||
					((p = strstr(buf,"sdd/sdd"))!= NULL) )
		return MSG_USB_REMOVE;
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
    if (retval < 0) {
        printf("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }
    return hotplug_sock;

}
INT32 usbEvent::getUsbName(const CHAR* path, struct dirent *file,UINT32 num)
{
	if(num <= 0 || path == NULL || file == NULL || (Caccess(path, F_OK) < 0))
	{
		return -1;
	}
	DIR* dp = NULL;
	UINT32 i = 0;
	struct dirent *filename = NULL;
	if((dp = opendir(path)) == NULL)
	{
		return -1;
	}
	while(((filename = readdir(dp)) != NULL) && (i < num))
	{
		printf("dirname: %s\n",filename->d_name);
		memcpy(file+i, filename, sizeof(struct dirent));
		i ++;
	}
	closedir(dp);
	return i;

}
INT32 usbEvent::getRaspCmdFile(struct dirent *file, INT32 num, /*out*/ CHAR* file_patch, INT32 file_patch_len)
{
	CHAR usb_name[256] = {0};
	INT32 ret = -1;
	if(file == NULL || num <= 0 || file_patch == NULL || file_patch_len <= 0)
	{
		return -1;
	}
	for(INT32 i = 0; i < num; i++)
	{
		if((strcmp((file + i)->d_name, ".") == 0) || (strcmp((file + i)->d_name, "..") == 0))
		{
			continue;
		}
		memset(usb_name, 0, sizeof(usb_name));
		strcat(usb_name,"/media/pi/");
		memcpy(usb_name + strlen("/media/pi/"), (file + i)->d_name, strlen((file + i)->d_name)+1);
		strcat(usb_name, "/");
		strcat(usb_name,RASPBERRY_COMMAND);
		printf("%s\n",usb_name);
		ret = access(usb_name, F_OK | R_OK | W_OK);
		if(ret == 0)
		{
			if(strlen(usb_name) + 1 > (UINT32)file_patch_len)
			{
				printf("name len is too long\n");
				return -2;
			}
			memcpy(file_patch, usb_name, strlen(usb_name) + 1);
			printf("find %s\n",RASPBERRY_COMMAND);
			return 0;
		}
	}
		return -1;

}
INT32 usbEvent::getSockid()
{
    return sockid;
}

INT32 usbEvent::readLine(FILE* fd, CHAR* data, INT32 len)
{
    if(fd == NULL || data == NULL || len <= 0)
    {
        return -1; 
    }
    if(feof(fd))
    {
        return 0;
    }
    return (strlen(fgets(data,len,fd)));
}

bool usbEvent::parseRaspFile(const CHAR* patch, Parse_val_t* param, INT32 len)
{
    if(patch == NULL || param == NULL || len <= 0)
    {
        return false;
    }
    FILE* fp = fopen(patch, "r+");
    if(fp == NULL)
    {
        return false;
    }
    INT32 i = 0;
    CHAR param_line[128] = {0};
    printf("{{parse data from file:\n");
    while((readLine(fp, param_line, 128) != 0) && (len > i))
    {
        (param + i)->bused = 1;
        (param + i)->type = (PARSE_TYPE)i;
        memcpy((param + i)->param, param_line, strlen(param_line) + 1);
        memset(param_line, 0, sizeof(param_line));
        i ++;
    }
    printf("\n{{parse data end}}}\n");
    fclose(fp);
    return true;
}

