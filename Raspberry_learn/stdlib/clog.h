#ifndef CLOG_H
#define CLOG_H
#include "common.h"
#include "cfile.h"
#include "mutexlock.h"

#define MAX_FILENAME_LEN 256
#define LOG_BUFF_SIZE    2048

//\e[显示方式;前景色;背景色m输出字符串\033[0m
#define AT_NONE            0
#define AT_HL              1
#define AT_NOBOLD          22
#define AT_UNDERSOCRE      4
#define AT_NOUNDERSOCRE    24
#define AT_REVERSE         7
#define AT_NOREVERSE       27

#define TC_BLACK    30
#define TC_RED      31
#define TC_GREEN    32
#define TC_YELLOW   33
#define TC_BLUE     34
#define TC_PURPLE   35
#define TC_CYAN     36
#define TC_WHITE    37

#define BC_BLACK    40
#define BC_RED      41
#define BC_GREEN    42
#define BC_YELLOW   43
#define BC_BLUE     44
#define BC_PURPLE   45
#define BC_CYAN     46
#define BC_WHITE    47

#define COLOR_SET(iStyle,iTC,iBC, str) COLOR_SET_(iStyle,iTC,iBC, str)
#define COLOR_SET_(iStyle,iTC,iBC, str) "\033[" #iStyle ";" #iTC ";" #iBC "m" str "\033[0m"
#define TEXT_COLOR_SET(color, str) COLOR_SET(AT_NONE,color,color, str)
#define COLOR_BLACK(str)       TEXT_COLOR_SET(TC_BLACK, str) 
#define COLOR_RED(str)         TEXT_COLOR_SET(TC_RED, str) 
#define COLOR_GREEN(str)       TEXT_COLOR_SET(TC_GREEN, str) 
#define COLOR_YELLOW(str)      TEXT_COLOR_SET(TC_YELLOW, str) 
#define COLOR_BLUE(str)        TEXT_COLOR_SET(TC_BLUE, str) 
#define COLOR_PURPLE(str)      TEXT_COLOR_SET(TC_PURPLE, str)
#define COLOR_CYAN(str)        TEXT_COLOR_SET(TC_CYAN, str)
#define COLOR_WHITE(str)       TEXT_COLOR_SET(TC_WHITE, str)

#define DBG_INFO
#ifdef DBG_INFO
#define DBG(fmt, ...) printf("%s()[%d] " fmt "\n",__FUNCTION__, __LINE__,##__VA_ARGS__)
#else
#define DBG(fmt, ...) printf("" fmt "\n",##__VA_ARGS__)
#endif

#define LOG_INFO_NOLOCK(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_INFO, __FILE__, __LINE__, COLOR_BLUE(format), ##__VA_ARGS__)

#define LOG_SOCKET_DEBUG(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_SOCKET_DEBUG, __FILE__, __LINE__, COLOR_BLUE(format), ##__VA_ARGS__)

#define LOG_DEBUG(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_DEBUG, __FILE__, __LINE__, COLOR_BLUE(format), ##__VA_ARGS__)

#define LOG_INFO(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_INFO, __FILE__, __LINE__, COLOR_GREEN(format), ##__VA_ARGS__)

#define LOG_WARNING(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_WARNING, __FILE__, __LINE__, COLOR_YELLOW(format), ##__VA_ARGS__)

#define LOG_ERROR(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_ERROR, __FILE__, __LINE__, COLOR_RED(format), ##__VA_ARGS__)

#define TINY_CHECK(exp) \
{ \
    if(!(exp)) \
    { \
        LOG_ERROR("File:%s, Line:%d Exp: %d is true, abort.\n",__FILE__,__LINE__,exp);abort(); \
    } \
}

#if 0
#define LOG_INFO_NOLOCK DBG
#define LOG_SOCKET_DEBUG DBG
#define LOG_DEBUG    DBG
#define LOG_INFO     DBG
#define LOG_WARNING  DBG
#define LOG_ERROR    DBG
#endif

typedef enum
{
    LEVEL_SOCKET_DEBUG,
    LEVEL_DEBUG,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR
}LOG_LEVEL;

class Clog
{
public:
    ~Clog();
    static Clog* getInstance();
    bool Init(const CHAR* fileName , INT32 miniLevel);
    bool Init(INT32 fd, INT32 miniLevel);
    VOID writeLog(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, ...);
    VOID writeLogNolock(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, ...);
private:
    Clog();
    VOID setLog(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, va_list valst);
    MutexLock mutex;
    Cfile logfile;
    INT32 minlevel;
    CHAR* logfilename;
    CHAR* logbuff;
};

#endif