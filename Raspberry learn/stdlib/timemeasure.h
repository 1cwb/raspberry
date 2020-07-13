#ifndef TIMEMEASURE_H
#define TIMEMEASURE_H
#include <sys/times.h>
#include <unistd.h>
#include "common.h"
class TimeMeasure
{
public:
    TimeMeasure();
    ~TimeMeasure();
    bool begin();
    bool end();
    DOUBLE getRealTime();
    DOUBLE getUserTime();
    DOUBLE getSysTime();
    DOUBLE getChdUserTime();
    DOUBLE getChdSysTime();
    VOID printAllTime();
private:
    TimeMeasure(const TimeMeasure&);
    struct tms tmsstart;
    struct tms tmsend;
    clock_t start;
    clock_t endtm;
    LONG clktck;
};

#endif
