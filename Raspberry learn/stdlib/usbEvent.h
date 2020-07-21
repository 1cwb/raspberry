#ifndef __USB_EVENT_H__
#define __USB_EVENT_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include "common.h"

#define USB_MOUNT_INFO    "/proc/mounts"
#define RASPBERRY_COMMAND "/RASPBERRY_CMD/Raspberry.cmd"

#define UEVENT_BUFFER_SIZE 1024
#define FILE_LINE_LEN 512
#define DIRECTORY_NUM  256
#define FILE_PATCH_LEN 256
#define USB_SDX_LEN 16
#define USB_PATCH_NAME_LEN 128
#define MAX_USB_DEV_NUM  12
typedef enum
{
GET_IP,

MAX_PARSE_TYPE
}PARSE_TYPE;

typedef struct
{
    CHAR bused;
    PARSE_TYPE type;
    CHAR param[128];
}Parse_val_t;
typedef struct usb_dev
{
    CHAR dev[USB_SDX_LEN];
    CHAR path_name[USB_PATCH_NAME_LEN];
    bool beused;
}usb_device_t;

class usbEvent
{
public:
    usbEvent();
    ~usbEvent();
    MSG_EVENT getUsbEvent(const CHAR* buf, CHAR* usb_patch_name, INT32 len);
    INT32 initHotplugSock(VOID);
    static bool parseRaspFile(const CHAR* patch, Parse_val_t* param, INT32 len);
    static INT32 readLine(FILE* fd, CHAR* data, INT32 len);
    bool getUSBPatchName(CHAR* usb_patch_name, INT32 len);
    INT32 getSockid();
    VOID showAllUsbPath();
private:
    bool addUsb(const CHAR* dev_name, const CHAR* path_name);
    bool removeUsb(const CHAR* dev_name);
    INT32 sockid;
    usb_device_t usb_path_info[MAX_USB_DEV_NUM];
};


#endif
