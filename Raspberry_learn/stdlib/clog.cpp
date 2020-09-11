#include "clog.h"
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
static CHAR* leveStr[5] = {(CHAR*)"DEBUG",(CHAR*)"DEBUG", (CHAR*)"INFO", (CHAR*)"WARN", (CHAR*)"ERROR"};

Clog::Clog() : minlevel(0), logfilename(NULL), logbuff(NULL)
{
    logfilename = (CHAR*)malloc(MAX_FILENAME_LEN);
    logbuff = (CHAR*)malloc(LOG_BUFF_SIZE);
    memset(logfilename, 0, MAX_FILENAME_LEN);
    memset(logbuff, 0, LOG_BUFF_SIZE);
}
Clog::~Clog()
{
    if(logfilename != NULL)
    {
        free(logfilename);
        logfilename = NULL;
    }
    if(logbuff != NULL)
    {
        free(logbuff);
        logbuff = NULL;
    }
}
Clog* Clog::getInstance()
{
    static Clog instance;
    return &instance;
}
bool Clog::Init(const CHAR* fileName, INT32 miniLevel)
{
    strncpy(logfilename, fileName, MAX_FILENAME_LEN - 1);
    if(miniLevel >= 0 && miniLevel <= 4)
    {
        minlevel = miniLevel;
    }
    logfile.Cfopen(logfilename, "a");
    if(!logfile.isFileopenSuccess())
    {
        return false;
    }
    return true;
}
bool Clog::Init(INT32 fd, INT32 miniLevel)
{
    strncpy(logfilename, "STDOUT", MAX_FILENAME_LEN - 1);
    if(miniLevel >= 0 && miniLevel <= 4)
    {
        minlevel = miniLevel;
    }
    logfile.Cfdopen(fd, "a");
    if(!logfile.isFileopenSuccess())
    {
        return false;
    }
    return true;
}
VOID Clog::writeLog(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, ...)
{
    if(level < minlevel)
    {
        return;
    }
    mutex.lock();
    va_list valst;
    va_start(valst, format);
    setLog(level, execfile, execline, format, valst);
    va_end(valst);

    logfile.Cfputs(logbuff);
    logfile.Cfflush();
    mutex.unlock();
}
VOID Clog::writeLogNolock(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, ...)
{
    if(level < minlevel)
    {
        return;
    }
    va_list valst;
    va_start(valst, format);
    setLog(level, execfile, execline, format, valst);
    va_end(valst);

    logfile.Cfputs(logbuff);
    logfile.Cfflush();
}
VOID Clog::setLog(LOG_LEVEL level, const CHAR* execfile, INT32 execline, const CHAR* format, va_list valst)
{
    CHAR exec_filename[MAX_FILENAME_LEN] = {0};
    const CHAR* pch = strrchr(execfile, '/');

    if(pch == NULL)
    {
        strncpy(exec_filename, execfile, MAX_FILENAME_LEN - 1);
    }
    else
    {
        strncpy(exec_filename, pch + 1, strlen(pch + 1));
    }

    memset(logbuff, 0, LOG_BUFF_SIZE);
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    struct  tm* sys_tm = localtime(&(now.tv_sec));

    INT32 n = snprintf(logbuff, 128, "\n%d-%02d-%02d %02d:%02d:%02d,%03ld <%s>[%s:%d] ",
                          sys_tm->tm_year + 1900, sys_tm->tm_mon + 1, sys_tm->tm_mday, sys_tm->tm_hour,
                          sys_tm->tm_min, sys_tm->tm_sec, now.tv_usec / 1000,
                          leveStr[level], exec_filename, execline);
    vsnprintf(logbuff + n, LOG_BUFF_SIZE - n, format, valst);
}
