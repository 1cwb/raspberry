#ifndef OLED_H
#define OLED_H
#include "common.h"
#include "cfileSys.h"
#include "fonts.h"
    //I2C
#define IIC_CMD        0X00
#define IIC_RAM        0X40
#define OLED_WIDTH     128
#define OLED_HEIGHT    32
#define OLED_COLUMN    OLED_WIDTH
#define OLED_PAGE      OLED_HEIGHT / 8

typedef enum{
    COLOR_WHITE = 0,
    COLOR_BLACK
}COLOR_TYPE;

class Oled
{
public:
    ~Oled();
    static Oled* getInstance();
    bool init(INT32 ch);
    VOID OLED_Clear();
    VOID drawPoint(INT32 x, INT32 y, COLOR_TYPE color = COLOR_WHITE);
    VOID disChar(INT32 x, INT32 y, const CHAR Acsii_Char, sFONT* Font = &Font12, COLOR_TYPE color = COLOR_WHITE);
    VOID disString(INT32 x, INT32 y, const CHAR * pString, sFONT* Font = &Font12, COLOR_TYPE color = COLOR_WHITE);
    VOID disCharOnTerminal(INT32 x, INT32 y, const CHAR Acsii_Char, sFONT* Font = &Font16);
    VOID disStringOnTerminal(INT32 x, INT32 y, const CHAR * pString, sFONT* Font = &Font16);
    VOID show();
private:
    Oled();
    static Oled oled_demo;
    INT32 i2cRead(UCHAR ucAddr, UCHAR *buf, INT32 iLen);
    INT32 i2cWrite(UCHAR ucAddr,const UCHAR *buf, INT32 iLen);
    bool I2C_Write_Byte(UCHAR value, UCHAR Cmd);
    bool OLED_WriteReg(UCHAR Reg);
    bool OLED_WriteData(UCHAR Data);
    bool OLED_InitReg();
    VOID setFrameBuff(INT32 x, INT32 y, COLOR_TYPE color);
    VOID clearBuff();
    CfileSys dev_file;
    UCHAR oled_frame[OLED_PAGE][OLED_WIDTH];
    COLOR_TYPE back_ground_color;
};

#include "msg.h"
#include "thread.h"
class OledRun
{
public:
    OledRun(Msg* msg);
    ~OledRun();
    bool run();
    static VOID* oledFunc(VOID *param);
    Oled* getOled();
    Msg* getOledMsg();
private:
    Oled *oled;
    Thread oled_thread;
    Msg* oled_msg;
};
#endif
