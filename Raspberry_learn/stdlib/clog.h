#ifndef CLOG_H
#define CLOG_H
#include "common.h"
#include "cfile.h"
#include "mutexlock.h"

#define MAX_FILENAME_LEN 256
#define LOG_BUFF_SIZE    2048

#define LOG_INFO_NOLOCK(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_SOCKET_DEBUG(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_SOCKET_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_WARNING(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_WARNING, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...)    \
    Clog::getInstance()->writeLog(LEVEL_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__)
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