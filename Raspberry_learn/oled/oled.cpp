#include "oled.h"

#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

const static UCHAR oled_init_data[] = {0xAE, 0x40, 0xB0, 0xC8, 0x81, 0xff, 0xa1, 0xa6, 0xa8, 0x1f, 0xd3,
                            0x00, 0xd5, 0xf0, 0xd9, 0x22, 0xda, 0x02, 0xdb, 0x49, 0x8d, 0x14, 0xaf};

Oled Oled::oled_demo;

Oled::Oled()
{
    //DBG();
}
Oled::~Oled()
{
    //DBG();
}

bool Oled::init(INT32 ch)
{
    CHAR filename[16] = {0};
    back_ground_color = COLOR_BLACK;
    sprintf(filename, "/dev/i2c-%d", ch);
    if(!dev_file.copen(filename, O_RDWR))
    {
        DBG("open %s fail!",filename);
        return false;
    }
    if (dev_file.Cioctl(I2C_SLAVE, 0x3c) < 0)
    {
	DBG("Failed to acquire bus for OLED matrix\n");
        DBG("%s",Cstrerror(Cgeterrno()));
        dev_file.cclose();
	return false;
    }
    OLED_InitReg();
    OLED_Clear();
    DBG("Oled_init Success");
    return true;
}

Oled* Oled::getInstance()
{
    return &oled_demo;
}

INT32 Oled::i2cRead(UCHAR ucAddr,UCHAR *buf, INT32 iLen)
{
    INT32 rc;
    rc = dev_file.cwrite(&ucAddr, 1);
    if (rc == 1)
    {
        rc = dev_file.cread(buf, iLen);
    }
    return rc;
}
INT32 Oled::i2cWrite(UCHAR ucAddr,const UCHAR *buf, INT32 iLen)
{
    UCHAR ucTemp[512];
    INT32 rc;

    if (iLen > 511 || iLen < 1 || buf == NULL)
    {
        return -1; // invalid write
    }

    ucTemp[0] = ucAddr; // send the register number first 
    memcpy(&ucTemp[1], buf, iLen); // followed by the data
    rc = dev_file.cwrite(ucTemp, iLen+1);
    return rc-1;
}
bool Oled::I2C_Write_Byte(UCHAR value, UCHAR Cmd)
{
    if(i2cWrite(Cmd, &value, 1) != 1)
    {
        DBG("iic_write fail");
        return false;
    }
    return true;
}
bool Oled::OLED_WriteReg(UCHAR Reg)
{
    return I2C_Write_Byte(Reg, IIC_CMD);
}
bool Oled::OLED_WriteData(UCHAR Data)
{
    return I2C_Write_Byte(Data, IIC_RAM);
}

bool Oled::OLED_InitReg()
{
    INT32 num = sizeof(oled_init_data);
    if(i2cWrite(IIC_CMD, oled_init_data, num) != num)
    {
        DBG("Fail:OLED_InitReg not successful!");
        return false;
    }
    return true;
}

VOID Oled::OLED_Clear()
{
    CHAR Page;
    clearBuff();
    for(Page = 0; Page < OLED_PAGE; Page++) 
    {
        OLED_WriteReg(0xb0 + Page);    //Set page address
        OLED_WriteReg(0x00);           //Set display position - column low address
        OLED_WriteReg(0x10);           //Set display position - column high address
        i2cWrite(IIC_RAM, *(oled_frame + Page), OLED_WIDTH);
    }
}
VOID Oled::drawPoint(INT32 x, INT32 y, COLOR_TYPE color)
{
    if(x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        DBG("Invalid piex");
        return;
    }
    setFrameBuff(x, y, color);
}
VOID Oled::setFrameBuff(INT32 x, INT32 y, COLOR_TYPE color)
{   
    if(color == COLOR_WHITE)
    {
        if(back_ground_color == COLOR_WHITE)
        {
            back_ground_color = COLOR_BLACK;
            OLED_Clear();
        }
        oled_frame[y/8][x] |= (0x01 << (y % 8));
	//printf("[%d][%d] %x [%d]\n",x/8,y,oled_frame[x/8][y],x%8);
    }
    else
    {
        if(back_ground_color == COLOR_BLACK)
        {
            back_ground_color = COLOR_WHITE;
            OLED_Clear();
        }
        oled_frame[y/8][x] &= (0xfe << (y % 8));
    }
}

VOID Oled::show()
{
    for(INT32 Page = 0; Page < OLED_PAGE; Page++) 
    {
        OLED_WriteReg(0xb0 + Page);    //Set page address
        OLED_WriteReg(0x00);           //Set display position - column low address
        OLED_WriteReg(0x10);           //Set display position - column high address
        i2cWrite(IIC_RAM, *(oled_frame + Page), OLED_WIDTH);
    }
    clearBuff();
}

VOID Oled::disChar(INT32 x, INT32 y, const CHAR Acsii_Char, sFONT* Font, COLOR_TYPE color)
{
    INT32 Page, Column;

    UINT32 Char_Offset =(Acsii_Char - ' ') * Font->Height *(Font->Width / 8 +(Font->Width % 8 ? 1 : 0));
    const UCHAR *ptr = &Font->table[Char_Offset];

    for(Page = 0; Page < Font->Height; Page ++ ) 
    {
        for(Column = 0; Column < Font->Width; Column ++ ) 
        {
            if(*ptr &(0x80 >>(Column % 8)))
            {
                drawPoint(x + Column, y + Page, color);
            }
            //One pixel is 8 bits
            if(Column % 8 == 7)
            {
                ptr++;
            }
                
        }/* Write a line */
        if(Font->Width % 8 != 0)
        {
            ptr++;
        }  
    }/* Write all */
}

VOID Oled::disString(INT32 x, INT32 y, const CHAR * pString, sFONT* Font, COLOR_TYPE color)
{
    INT32 Xpoint = x;
    INT32 Ypoint = y;

    while(* pString != '\0') 
    {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
        if((Xpoint + Font->Width ) > OLED_WIDTH) 
        {
            Xpoint = x;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if((Ypoint  + Font->Height ) > OLED_HEIGHT) 
        {
            Xpoint = x;
            Ypoint = y;
        }
        disChar(Xpoint, Ypoint, * pString, Font, color);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}
VOID Oled::clearBuff()
{
    if(back_ground_color == COLOR_WHITE)
    {
        memset(oled_frame, 0xff, sizeof(oled_frame));
    }
    else
    {
        memset(oled_frame, 0x00, sizeof(oled_frame));
    }
}
VOID Oled::disCharOnTerminal(INT32 x, INT32 y, const CHAR Acsii_Char, sFONT* Font)
{
    INT32 Page, Column;

    UINT32 Char_Offset =(Acsii_Char - ' ') * Font->Height *(Font->Width / 8 +(Font->Width % 8 ? 1 : 0));
    const UCHAR *ptr = &Font->table[Char_Offset];

    for(Page = 0; Page < Font->Height; Page ++ ) 
    {
        for(Column = 0; Column < Font->Width; Column ++ ) 
        {
            if(*ptr &(0x80 >>(Column % 8)))
            {
		printf("o");
            }
	    else
	    {
	    	printf("*");
	    }
            //One pixel is 8 bits
            if(Column % 8 == 7)
            {
                ptr++;
            }
        }/* Write a line */
	    printf("\n");
        if(Font->Width % 8 != 0)
        {
            ptr++;
        } 
    }/* Write all */
}
VOID Oled::disStringOnTerminal(INT32 x, INT32 y, const CHAR * pString, sFONT* Font)
{
    INT32 Xpoint = x;
    INT32 Ypoint = y;

    while(* pString != '\0') 
    {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
        if((Xpoint + Font->Width ) > OLED_WIDTH) 
        {
            Xpoint = x;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if((Ypoint  + Font->Height ) > OLED_HEIGHT) 
        {
            Xpoint = x;
            Ypoint = y;
        }
        disCharOnTerminal(Xpoint, Ypoint, * pString, Font);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}


OledRun::OledRun(Msg* msg)
{
    this->oled_msg = msg;
    oled = Oled::getInstance();
    oled->init(1);
}
OledRun::~OledRun()
{
    oled_thread.join();
}
Oled* OledRun::getOled()
{
    return oled;
}
Msg* OledRun::getOledMsg()
{
    return oled_msg;
}
bool OledRun::run()
{
    oled_thread.setThreadFunc(oledFunc, (VOID*)this);
    oled_thread.start();
    return true;
}

VOID* OledRun::oledFunc(VOID *param)
{
    Oled* oled = ((OledRun*) param)->oled;
    Msg*  oled_msg = ((OledRun*) param)->oled_msg;
    CHAR ethIP[24] = {0};
    CHAR wlanIP[24] = {0};
    CHAR usbIP[24] = {0};
    CHAR* cmd_buff = (char*)malloc(2048);
    INT32 num = 0;
    if(oled_msg != NULL && oled != NULL)
    {
        queue_buf *oled_data = NULL;
        cntl_queue_ret msg_ret = CNTL_QUEUE_INVALIED;
        while(msg_ret != CNTL_QUEUE_CANCEL)
        {
            msg_ret = oled_msg->pop((VOID**)&oled_data,10000000, IPC_WAITTIMES);
            if(oled_data == NULL)
            {
                DBG("oled data is NULL");
                continue;
            }
            else
            {
                switch(oled_data->msg_type)
                {
                    case MSG_USB_ADD:
                    case MSG_USB_REMOVE:
                        oled->disString(0,0,oled_data->msg_buf);
                        oled->show();
			//DBG("---------%s",oled_data->msg_buf);
                        break;
                    case MSG_UPDATE_IP:
                        memset(cmd_buff, 0, 2048);
                        memset(ethIP, 0, sizeof(ethIP));
                        memset(wlanIP, 0, sizeof(wlanIP));
                        memset(usbIP, 0, sizeof(usbIP));
                        DoComman("ifconfig", "r", cmd_buff, 2048);
                        GetIPFromIfconfig(cmd_buff, "eth[0-9]", ethIP, sizeof(ethIP));
                        GetIPFromIfconfig(cmd_buff, "wlan[0-9]", wlanIP, sizeof(wlanIP));
                        GetIPFromIfconfig(cmd_buff, "usb[0-9]", usbIP, sizeof(usbIP));
                        break;
                    case MSG_SHOW_IP:
                        break;
                    case MSG_WRITE_MSG_TO_DISK_OK:
                    case MSG_WRITE_MSG_TO_DISK_FAIL:
                        oled->disString(0,0,oled_data->msg_buf);
                        oled->show();
                        break;
                    default:            
                        if(num == 5)
                        {
                            oled->disString(0,0,ethIP);
                            oled->show();
                        }
                        else if(num == 10)
                        {
                            oled->disString(0,0,wlanIP);
                            oled->show();
                        }                    
                        else if(num == 15)
                        {
                            oled->disString(0,0,usbIP);
                            oled->show();
                        }
                        else if(num == 20)
                        {
                            oled->disString(0,0,oled_data->msg_buf);
                            oled->show();
                            num = 0;
                        }
                        num ++;
                        break;
                }
            }
            free(oled_data);
            oled_data = NULL;
        }
    }
    free(cmd_buff);
    return NULL;
}
    //Oled oled;
    //Thread oled_thread;
    //Msg* oled_msg;
